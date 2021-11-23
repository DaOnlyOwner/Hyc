#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"
#include "DebugPrint.h"
#include <cstdlib>
#include "Ast.h"
#include <memory>


// Expressions

#define DEF_POSTFIX_OP(name,group,right)\
InfixOperation<Expr> name{ (int)ExprPrecedence::group, right, [](InfixExprFnArgs) {\
		return std::make_unique<PostfixOpExpr>(token,mv(lh));} }

#define DEF_BIN_OP(name, group, right)\
InfixOperation<Expr> name{(int)ExprPrecedence::group, right, [](InfixExprFnArgs){\
		return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal((int)ExprPrecedence::group));\
}}

#define DEF_PREFIX_OP(name, group)\
PrefixOperation<Expr> name{(int)ExprPrecedence::group, [](PrefixExprFnArgs)\
	{\
		return static_cast<uptr<Expr>>(std::make_unique<PrefixOpExpr>(Token(token),parser.parse_internal((int)ExprPrecedence::group)));\
	} }

#define ADD_OP(op,name) expr_parser.add_operation(op,name)

namespace
{

	std::pair<bool, EvalIntegerResult> eval_integer_val(const Token& token)
	{
		const std::string& integer_text = token.text;
		errno = 0;
		auto val = strtoull(token.text.c_str(), nullptr, 0);
		if (errno != 0)
		{
			return { false,{Primitive::Specifier::Count,0,0 } };
		}
		return { true,{Primitive::Specifier::uint,val,0 } };
	}

	PrefixOperation<Expr> integer_lit{(int)ExprPrecedence::Group0, [](PrefixExprFnArgs)
	{
			auto [succ,eval_res] = eval_integer_val(token);
			if (!succ)
			{
				auto descr = Error::FromToken(token);
				descr.Message = fmt::format("{} doesn't fit into an i64 or u64", token.text);
				descr.Hint = "You need to reduce the size of the literal, so that it fits into either an i64 or an u64";
				Error::SyntacticalError(descr);
			}
		return ast_as<Expr>(std::make_unique<IntegerLiteralExpr>(token,eval_res));
	} };

	PrefixOperation<Expr> ident{ (int)ExprPrecedence::Group0, [](PrefixExprFnArgs)
	{
			auto& l = parser.overall_parser->get_lexer();
			uptr<Expr> ident_expr = std::make_unique<IdentExpr>(Token(token));
			// It's actually a function call
			if (l.lookahead(1).type == Token::Specifier::Less)
			{
				std::vector<uptr<TypeSpec>> generic_params;
				l.eat(); // <
				auto fst = parser.overall_parser->parse_type_spec();
				generic_params.push_back(mv(fst));
				while (l.lookahead(1).type == Token::Specifier::Comma)
				{
					l.eat(); // ,
					generic_params.push_back(parser.overall_parser->parse_type_spec());
				}
				l.match_token(Token::Specifier::Greater); // >
				l.match_token(Token::Specifier::RParenL);
				std::vector<uptr<Expr>> arg_list;
				// Parse an argument list:
				while (l.lookahead(1).type != Token::Specifier::RParenR && l.lookahead(1).type != Token::Specifier::Eof)
				{
					arg_list.push_back(parser.parse());
					if (l.lookahead(1).type == Token::Specifier::Comma)
						l.eat();
				}

				l.match_token(Token::Specifier::RParenR); // ')'

				return static_cast<uptr<Expr>>(std::make_unique<FuncCallExpr>(mv(ident_expr), mv(arg_list), mv(generic_params)));
			}
			return ident_expr;
	} };

	// Precedence  right_assoc?  Parsing function
	DEF_BIN_OP(scope, Group1, false);
	DEF_POSTFIX_OP(incr_decr, Group2, false);
	InfixOperation<Expr> func_call_array_sub{(int)ExprPrecedence::Group2, false, [](InfixExprFnArgs) {
		// Function call
		Lexer& l = parser.overall_parser->get_lexer();
		if(token.type == Token::Specifier::RParenL)
		{
			std::vector<uptr<Expr>> arg_list;
			// Parse an argument list:
			while (l.lookahead(1).type != Token::Specifier::RParenR && l.lookahead(1).type != Token::Specifier::Eof)
			{
				arg_list.push_back(parser.parse());
				if (l.lookahead(1).type == Token::Specifier::Comma)
					l.eat();
			}

			l.match_token(Token::Specifier::RParenR); // ')'

			return static_cast<uptr<Expr>>(std::make_unique<FuncCallExpr>(mv(lh), mv(arg_list),std::vector<uptr<TypeSpec>>{}));
		}

		else if (token.type == Token::Specifier::BracketL)
		{
			auto expr = parser.parse();
			l.match_token(Token::Specifier::BracketR);
			return static_cast<uptr<Expr>>(std::make_unique<ArraySubscriptExpr>(mv(lh), mv(expr)));
		}
	} };
	DEF_BIN_OP(member, Group2, false);
	DEF_PREFIX_OP(unary, Group3);
	DEF_BIN_OP(cast, Group3, false);
	DEF_BIN_OP(ind_addr_of, Group3, true);
	DEF_BIN_OP(mult, Group5, false);
	DEF_BIN_OP(add, Group6, false);
	DEF_BIN_OP(shift, Group7, false);
	DEF_BIN_OP(threeway, Group8, false);
	DEF_BIN_OP(rel, Group9, false);
	DEF_BIN_OP(comp, Group10, false);
	DEF_BIN_OP(and_, Group11, false);
	DEF_BIN_OP(xor_, Group12, false);
	DEF_BIN_OP(or_ , Group13, false);
	DEF_BIN_OP(land, Group14, false);
	DEF_BIN_OP(lor, Group15, false);

	InfixOperation<Expr> if_expr{ (int)ExprPrecedence::Group16, true, [](InfixExprFnArgs) {
		auto snd = parser.parse();
		parser.overall_parser->get_lexer().match_token(Token::Specifier::Colon);
		auto trd = parser.parse_internal((int)ExprPrecedence::Group16);
		return std::make_unique<TernaryExpr>(mv(lh), mv(snd), mv(trd));
	} };

	DEF_BIN_OP(assignment_move_etc, Group16, true);
	DEF_PREFIX_OP(throw_, Group16);	
}

Parser::Parser(Lexer& token_source, const std::string& filename)
	:expr_parser(this,token_source),
	tkns(token_source)
{
	expr_parser.add_operation(Token::Specifier::DoubleColon, scope);
	expr_parser.add_operation(Token::Specifier::DoublePlus, incr_decr);
	ADD_OP(Token::Specifier::DoubleMinus, incr_decr);
	ADD_OP(Token::Specifier::RParenL, func_call_array_sub);
	ADD_OP(Token::Specifier::BracketL, func_call_array_sub);
	ADD_OP(Token::Specifier::Dot, member);
	ADD_OP(Token::Specifier::MemAccess, member);
	ADD_OP(Token::Specifier::Minus, unary);
	ADD_OP(Token::Specifier::Plus, unary);
	ADD_OP(Token::Specifier::ExclMark, unary);
	ADD_OP(Token::Specifier::Tilde, unary);
	ADD_OP(Token::Specifier::KwAs, cast);
	ADD_OP(Token::Specifier::Asterix, unary);
	ADD_OP(Token::Specifier::Ampersand, unary);
	ADD_OP(Token::Specifier::Percent, unary);
	ADD_OP(Token::Specifier::Asterix, mult);
	ADD_OP(Token::Specifier::Slash, mult);
	ADD_OP(Token::Specifier::Percent, mult);
	ADD_OP(Token::Specifier::Plus, add);
	ADD_OP(Token::Specifier::Minus, add);
	ADD_OP(Token::Specifier::ShiftLeft, shift);
	ADD_OP(Token::Specifier::ShiftRight, shift);
	ADD_OP(Token::Specifier::ThreeWay, threeway);
	ADD_OP(Token::Specifier::Less, rel);
	ADD_OP(Token::Specifier::Greater, rel);
	ADD_OP(Token::Specifier::LessEql, rel);
	ADD_OP(Token::Specifier::GreaterEql, rel);
	ADD_OP(Token::Specifier::DoubleEqual, comp);
	ADD_OP(Token::Specifier::NotEqual, comp);
	ADD_OP(Token::Specifier::Ampersand, and_);
	ADD_OP(Token::Specifier::Caret, xor_);
	ADD_OP(Token::Specifier::Or, or_);
	ADD_OP(Token::Specifier::DoubleAmpersand, land);
	ADD_OP(Token::Specifier::DoubleOr, lor);
	ADD_OP(Token::Specifier::QuestionMark, if_expr);
	ADD_OP(Token::Specifier::Equal, assignment_move_etc);
	ADD_OP(Token::Specifier::PlusEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::MinusEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::AsterixEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::SlashEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::SlEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::SrEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::PercentEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::AmpersandEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::CaretEqual, assignment_move_etc);
	ADD_OP(Token::Specifier::OrEqual, assignment_move_etc);
	// TODO: Add #*, #/, #+ etc.
	ADD_OP(Token::Specifier::KwThrow, throw_);
	ADD_OP(Token::Specifier::Ident, ident);
	ADD_OP(Token::Specifier::Integer, integer_lit);

	file = filename;
}

std::unique_ptr<Stmt> Parser::parse()
{
	return parse_compilation_unit();
}

// stmt*
std::unique_ptr<Stmt> Parser::parse_compilation_unit()
{
	auto nms = std::make_unique<NamespaceStmt>(Token(Token::Specifier::Ident, "GLOBAL", file, "", 0, 0, 0, 0));
	while (tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		nms->stmts.push_back(parse_allowed_namespace_stmt());
	}
	return mv(nms);
}

// (type ident ((:= | :# | :) expr)?) | (type? ident (:= | :# | :) expr)  ; 
std::unique_ptr<Stmt> Parser::parse_decl_operator_stmt()
{
	auto la2 = tkns.lookahead(2).type;
	std::unique_ptr<TypeSpec> type = nullptr;
	if (la2 != Token::Specifier::DeclCpy && la2 != Token::Specifier::DeclMv && la2 != Token::Specifier::Colon)
		type = parse_type_spec(); // type
	auto& ident = tkns.match_token(Token::Specifier::Ident); // Ident
	auto la1 = tkns.lookahead(1).type;
	std::unique_ptr<Expr> expr;
	if (la1 == Token::Specifier::DeclCpy)
	{
		tkns.eat(); // :=
		expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::Semicolon); // ;
		return std::make_unique<DeclCpyStmt>(std::move(type), std::move(ident), std::move(expr));
	}

	else if (la1 == Token::Specifier::DeclMv)
	{
		tkns.eat(); // :#
		expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::Semicolon); // ;
		return std::make_unique<DeclMvStmt>(std::move(type), std::move(ident), std::move(expr));
	}

	else if (la1 == Token::Specifier::Colon)
	{
		tkns.eat(); // :
		expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::Semicolon); // ;
		return std::make_unique<DeclInitStmt>(std::move(type), std::move(ident), std::move(expr));
	}
	// Is a declaration stmt
	else if (la1 == Token::Specifier::Semicolon)
	{
		assert(type != nullptr); // It would look like this "a;" which is an expression. Contrary to the grammar spec we should never step into this case in this function
		tkns.match_token(Token::Specifier::Semicolon); // ; 
		return std::make_unique<DeclStmt>(std::move(type), std::move(ident));
	}
}

// type ident(< generic_list >)? (type ident1, type ident2,...,){stmts}
std::unique_ptr<Stmt> Parser::parse_function_def_stmt()
{
	auto func_decl = parse_function_decl_stmt_part();
	return std::make_unique<FuncDefStmt>(mv(func_decl), parse_function_body());
}

// TODO: Parse e.g. A::B
std::unique_ptr<TypeSpec> Parser::parse_type_spec_part()
{
	if (tkns.lookahead(1).type == Token::Specifier::Asterix)
	{
		tkns.eat();
		return std::make_unique<PointerTypeSpec>(parse_type_spec_part());
	}
	else if (tkns.lookahead(1).type == Token::Specifier::BracketL)
	{
		tkns.eat(); // [
		Token integer = tkns.match_token(Token::Specifier::Integer); // integer
		tkns.match_token(Token::Specifier::BracketR); // ]
		auto [succ, res] = eval_integer_val(integer);
		if (!succ)
		{
			auto descr = Error::FromToken(integer);
			descr.Message = fmt::format("{} doesn't fit into an i64 or u64", integer.text);
			descr.Hint = "You need to reduce the size of the literal, so that it fits into either an i64 or an u64";
			Error::SyntacticalError(descr);
		}
		if (res.Spec != Primitive::Specifier::uint)
		{
			auto descr = Error::FromToken(integer);
			descr.Message = fmt::format("Array size specificators must be of type uint, but {} has type int", integer.text);
			Error::SyntacticalError(descr);
		}
		auto integerLiteral = std::make_unique<IntegerLiteralExpr>(integer, res);
		return std::make_unique<ArrayTypeSpec>(mv(integerLiteral), parse_type_spec_part());
	}
	else return nullptr;
}

// type_spec := (ident(< ident* >)? | fptr(type_spec*;type_spec) | "*" | "[integer_literal]") type_spec
std::unique_ptr<TypeSpec> Parser::parse_type_spec()
{
	if (tkns.lookahead(1).type == Token::Specifier::Ident || tkns.lookahead(1).type == Token::Specifier::KwAuto)
	{
		std::vector<uptr<TypeSpec>> generics;
		auto& name = tkns.eat();
		if (tkns.lookahead(1).type == Token::Specifier::Less)
		{
			tkns.eat(); // <
			auto fst = parse_type_spec();
			generics.push_back(mv(fst));
			while (tkns.lookahead(1).type == Token::Specifier::Comma)
			{
				tkns.eat(); // ","
				generics.push_back(parse_type_spec());
			}

			tkns.match_token(Token::Specifier::Greater);

		} // Generics
		uptr<TypeSpec> scope = nullptr;
		if (tkns.lookahead(1).type == Token::Specifier::DoubleColon)
		{
			tkns.eat();
			scope = parse_type_spec();
		}
		if (scope != nullptr)
		{
			return std::make_unique<ScopeTypeSpec>(mv(scope),
				std::make_unique<BaseTypeSpec>(mv(name), nullptr, std::move(generics)));
		}
		auto inner = parse_type_spec_part();
		return std::make_unique<BaseTypeSpec>(mv(name), mv(inner), std::move(generics));
	}
	else if (tkns.lookahead(1).type == Token::Specifier::KwFptr)
	{
		tkns.eat();
		tkns.match_token(Token::Specifier::RParenL);
		std::vector<uptr<TypeSpec>> args;
		while (tkns.lookahead(1).type != Token::Specifier::Semicolon && tkns.lookahead(1).type != Token::Specifier::Eof)
		{
			args.push_back(parse_type_spec()); 
			if (tkns.lookahead(1).type == Token::Specifier::Comma)
				tkns.eat();
			else if (tkns.lookahead(1).type != Token::Specifier::Semicolon) break;
		}
		tkns.match_token(Token::Specifier::Semicolon);
		auto ret_type = parse_type_spec();
		tkns.match_token(Token::Specifier::RParenR);
		return std::make_unique<FptrTypeSpec>(mv(args), mv(ret_type));
	}
	return nullptr;
}

// type ident ;
std::unique_ptr<Stmt> Parser::parse_decl_stmt()
{
	auto type = parse_type_spec();
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<DeclStmt>(mv(type), mv(name));
}


// ident ("=" type)? (","(ident ("=" type)? ",")*)?
std::vector<GenericInfo> Parser::parse_comma_separated_ident_list()
{
	std::vector<GenericInfo> vec;
	auto& fst = tkns.match_token(Token::Specifier::Ident);
	GenericInfo fst_gi;
	fst_gi.name = mv(fst);

	if (tkns.is_at(Token::Specifier::Equal,1))
	{
		tkns.eat(); // =
		fst_gi.default_type = parse_type_spec();
	}

	vec.push_back(mv(fst_gi));

	while (tkns.lookahead(1).type == Token::Specifier::Comma)
	{
		tkns.eat();
		GenericInfo gi;
		auto& name = tkns.match_token(Token::Specifier::Ident);
		gi.name = mv(name);
		if (tkns.lookahead(1).type == Token::Specifier::Equal)
		{
			tkns.eat(); // =
			gi.default_type = parse_type_spec();
		}
		vec.push_back(mv(gi));
	}
	return vec;
}

// struct ident< comma_separated_ident_list > { declaration_stmt* }
std::unique_ptr<Stmt> Parser::parse_struct_def()
{
	return parse_attr_collection<StructDefStmt>(Token::Specifier::KwStruct, [&]() {return parse_decl_stmt(); });
}

std::unique_ptr<Stmt> Parser::parse_union_def()
{
	return parse_attr_collection<UnionDefStmt>(Token::Specifier::KwUnion, [&]() {return parse_decl_stmt(); });
}

// namespace ident { allowed_namespace_stmts }
std::unique_ptr<Stmt> Parser::parse_namespace_stmt()
{
	tkns.match_token(Token::Specifier::KwNamespace);
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> stmts = parse_allowed_namespace_stmts();
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<NamespaceStmt>(mv(stmts),mv(name));
}


std::unique_ptr<Stmt> Parser::parse_allowed_namespace_stmt()
{
	if (tkns.lookahead(3).type == Token::Specifier::Semicolon)
		return parse_decl_stmt();
	else if (tkns.lookahead(1).type == Token::Specifier::KwStruct)
		return parse_struct_def();
	else if (tkns.lookahead(1).type == Token::Specifier::KwNamespace)
		return parse_namespace_stmt();
	else if (tkns.lookahead(1).type == Token::Specifier::KwUnion)
		return parse_union_def();
	else return parse_function_def_stmt();
}

// decl_stmt | function_def | struct_def | namespace_stmt | contract_def | contract_impl
std::vector<std::unique_ptr<Stmt>> Parser::parse_allowed_namespace_stmts()
{
	std::vector<std::unique_ptr<Stmt>> vec;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		vec.push_back(parse_allowed_namespace_stmt());
	}
	return vec;
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_function_body()
{
	std::vector<uptr<Stmt>> body;

	tkns.match_token(Token::Specifier::BraceL); // '{'
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		body.push_back(parse_allowed_func_stmt());
	}

	tkns.match_token(Token::Specifier::BraceR); // '}'
	return body;
}

std::unique_ptr<FuncDeclStmt> Parser::parse_function_decl_stmt_part()
{
	auto type = parse_type_spec(); // Type

	Token name;
	std::vector<GenericInfo> generic_list;
	if (tkns.lookahead(1).type == Token::Specifier::KwOperator)
	{
		tkns.eat();
		auto& tkn = tkns.match_one_of < 
			Token::Specifier::DoublePlus,
			Token::Specifier::DoubleMinus,
			Token::Specifier::KwAs,
			Token::Specifier::Plus,
			Token::Specifier::Minus,
			Token::Specifier::ExclMark,
			Token::Specifier::Tilde,
			Token::Specifier::Asterix,
			Token::Specifier::Ampersand,
			Token::Specifier::Slash,
			Token::Specifier::Percent,
			Token::Specifier::ShiftLeft,
			Token::Specifier::ShiftRight,
			Token::Specifier::ThreeWay,
			Token::Specifier::Less,
			Token::Specifier::Greater,
			Token::Specifier::LessEql,
			Token::Specifier::GreaterEql,
			Token::Specifier::DoubleEqual,
			Token::Specifier::NotEqual,
			Token::Specifier::Caret,
			Token::Specifier::Or,
			Token::Specifier::DoubleAmpersand,
			Token::Specifier::DoubleOr,
			Token::Specifier::Equal,
			Token::Specifier::PlusEqual,
			Token::Specifier::MinusEqual,
			Token::Specifier::AsterixEqual,
			Token::Specifier::SlashEqual,
			Token::Specifier::PercentEqual,
			Token::Specifier::SlEqual,
			Token::Specifier::SrEqual,
			Token::Specifier::AmpersandEqual,
			Token::Specifier::CaretEqual,
			Token::Specifier::OrEqual > ();

		name = tkn;
		name.text = fmt::format("${}", tkn.text);
	}
	else
	{
		name = tkns.match_token(Token::Specifier::Ident); // ident

		if (tkns.lookahead(1).type == Token::Specifier::Less) // <
		{
			tkns.eat();
			generic_list = parse_comma_separated_ident_list();
			tkns.match_token(Token::Specifier::Greater);
		}
	}

	std::vector<std::pair<uptr<TypeSpec>, Token>> param_list;

	tkns.match_token(Token::Specifier::RParenL);

	while (!(tkns.lookahead(1).type == Token::Specifier::RParenR || tkns.lookahead(1).type == Token::Specifier::Eof))
	{
		auto arg_type = parse_type_spec(); // Type
		auto& arg_name = tkns.match_token(Token::Specifier::Ident); // name

		param_list.push_back(std::make_pair(std::move(arg_type), std::move(arg_name)));

		if (tkns.lookahead(1).type == Token::Specifier::Comma) tkns.eat();
	}

	tkns.match_token(Token::Specifier::RParenR); // ')'

	return std::make_unique<FuncDeclStmt>(mv(type), mv(name), mv(generic_list), mv(param_list));
}

std::unique_ptr<Stmt> Parser::parse_function_decl_stmt()
{
	auto out = parse_function_decl_stmt_part();
	tkns.match_token(Token::Specifier::Semicolon);
	return out;
}

std::unique_ptr<Stmt> Parser::parse_match_stmt(bool in_loop)
{
	tkns.match_token(Token::Specifier::KwMatch);
	tkns.match_token(Token::Specifier::RParenL);
	auto match_expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR);
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<MatchCase> cases_lst;
	while (!tkns.is_at(Token::Specifier::BraceR, 1) && !tkns.is_at(Token::Specifier::Eof, 1)) // is_at is an alternative to lookahead.type == Specifier
	{
		cases_lst.push_back(parse_match_case(in_loop));
	}
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<MatchStmt>(mv(cases_lst), mv(match_expr));
}

MatchCase Parser::parse_match_case(bool in_loop)
{
	tkns.match_token(Token::Specifier::KwCase);
	auto type = parse_type_spec();
	tkns.match_token(Token::Specifier::KwAs);
	auto name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> body;
	if (in_loop) body = parse_allowed_loop_stmts();
	else body = parse_allowed_func_stmts();
	tkns.match_token(Token::Specifier::BraceR);
	return MatchCase(std::make_unique<DeclStmt>(mv(type), mv(name)), mv(body));
}

std::unique_ptr<Stmt> Parser::parse_continue_stmt()
{
	tkns.match_token(Token::Specifier::KwContinue);
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<ContinueStmt>();
}

std::unique_ptr<Stmt> Parser::parse_allowed_loop_stmt()
{
	auto la1 = tkns.lookahead(1).type;
	auto la2 = tkns.lookahead(2).type;
	auto la3 = tkns.lookahead(3).type;
	switch (la1)
	{
	case Token::Specifier::KwMatch:
		return parse_match_stmt(true);
	case Token::Specifier::KwContinue:
		return parse_continue_stmt();
	case Token::Specifier::KwReturn:
		return parse_return_stmt();
	case Token::Specifier::KwFor:
		return parse_for_loop_stmt();
	case Token::Specifier::KwWhile:
		return parse_while_loop_stmt();
	case Token::Specifier::KwIf:
		return parse_if_stmt();
	case Token::Specifier::Ident:
	{
		if (la2 == Token::Specifier::Asterix // e.g. int*
			|| la2 == Token::Specifier::Less // e.g. List<
			|| la2 == Token::Specifier::BracketL // e.g. int[
			|| la2 == Token::Specifier::DeclCpy // e.g. a :=
			|| la2 == Token::Specifier::DeclMv // e.g. a :#
			|| la2 == Token::Specifier::Colon // e.g. a :
			|| la3 == Token::Specifier::DeclCpy // int a :=
			|| la3 == Token::Specifier::DeclMv // int b :#
			|| la3 == Token::Specifier::Colon)  // int c : 
			return parse_decl_operator_stmt(); // Also parses decl
		else if (la2 == Token::Specifier::Ident) // int a;
			return parse_decl_stmt();
	}
	break;
	case Token::Specifier::KwFptr:
		return parse_decl_stmt();
	default:
		break;
	}
	return parse_expr_stmt();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_allowed_loop_stmts()
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		stmts.push_back(parse_allowed_loop_stmt());
	}
	return stmts;
}

// Should only be called from whatever has "}" as closing token
std::vector<std::unique_ptr<Stmt>> Parser::parse_func_def_list()
{
	std::vector<uptr<Stmt>> stmts;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		stmts.push_back(parse_function_def_stmt());
	}
	return stmts;
}

// e.g if(a) {b;} elif(c){} else{}
std::unique_ptr<Stmt> Parser::parse_if_stmt()
{
	tkns.match_token(Token::Specifier::KwIf);
	tkns.match_token(Token::Specifier::RParenL);
	auto if_expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR);
	std::vector<uptr<Stmt>> if_stmts;
	if (tkns.lookahead(1).type == Token::Specifier::BraceL)
	{
		tkns.eat();
		if_stmts = parse_allowed_func_stmts();
		tkns.match_token(Token::Specifier::BraceR);
	}

	else if (tkns.lookahead(1).type != Token::Specifier::KwElif && tkns.lookahead(1).type != Token::Specifier::KwElse)
	{
		if_stmts.push_back(parse_allowed_func_stmt());
	}

	std::vector<std::vector<uptr<Stmt>>> all_elif_stmts;
	std::vector<uptr<Expr>> elif_exprs;
	while (tkns.lookahead(1).type == Token::Specifier::KwElif)
	{
		uptr<Expr> elif_expr;
		std::vector<uptr<Stmt>> elif_stmts;
		tkns.eat(); // elif
		tkns.match_token(Token::Specifier::RParenL);
		elif_expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::RParenR);

		if (tkns.lookahead(1).type == Token::Specifier::BraceL)
		{
			tkns.eat(); // {
			elif_stmts = parse_allowed_func_stmts();
			tkns.match_token(Token::Specifier::BraceR);
		}
		else if (tkns.lookahead(1).type != Token::Specifier::KwElif && tkns.lookahead(1).type != Token::Specifier::KwElse)
		{
			elif_stmts.push_back(parse_allowed_func_stmt());
		}

		all_elif_stmts.push_back(std::move(elif_stmts));
		elif_exprs.push_back(std::move(elif_expr));
	}

	std::vector<uptr<Stmt>> else_stmts;
	if (tkns.lookahead(1).type == Token::Specifier::KwElse)
	{
		tkns.eat(); // else

		if (tkns.lookahead(1).type == Token::Specifier::BraceL)
		{
			tkns.eat(); // {
			else_stmts = parse_allowed_func_stmts();
			tkns.match_token(Token::Specifier::BraceR);
		}
		else if (tkns.lookahead(1).type != Token::Specifier::KwElif && tkns.lookahead(1).type != Token::Specifier::KwElse)
		{
			else_stmts.push_back(parse_allowed_func_stmt());
		}
	}
	
	return std::make_unique<IfStmt>(mv(if_expr), mv(if_stmts), mv(elif_exprs), mv(all_elif_stmts), mv(else_stmts));
}


std::unique_ptr<Stmt> Parser::parse_while_loop_stmt()
{
	tkns.match_token(Token::Specifier::KwWhile);
	tkns.match_token(Token::Specifier::RParenL);
	auto expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR);
	tkns.match_token(Token::Specifier::BraceL);
	auto stmts = parse_allowed_loop_stmts(); 
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<WhileStmt>(mv(expr), mv(stmts));
}

std::unique_ptr<Stmt> Parser::parse_for_loop_stmt()
{
	tkns.match_token(Token::Specifier::KwFor); // for
	tkns.match_token(Token::Specifier::RParenL); // (
	auto decl_stmt = parse_decl_operator_stmt(); // This parses the semicolon
	auto fst_expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::Semicolon);
	auto snd_expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR); // )
	tkns.match_token(Token::Specifier::BraceL); // {
	auto stmts = parse_allowed_loop_stmts();
	tkns.match_token(Token::Specifier::BraceR); // }
	return std::make_unique<ForStmt>(mv(decl_stmt), mv(fst_expr), mv(snd_expr),mv(stmts));
}

// var_decl_def | expr_stmt | return_stmt
std::unique_ptr<Stmt> Parser::parse_allowed_func_stmt()
{
	auto la1 = tkns.lookahead(1).type;
	auto la2 = tkns.lookahead(2).type;
	auto la3 = tkns.lookahead(3).type;
	switch (la1)
	{
	case Token::Specifier::KwMatch:
		return parse_match_stmt(false);
	case Token::Specifier::KwReturn:
		return parse_return_stmt();
	case Token::Specifier::KwFor:
		return parse_for_loop_stmt();
	case Token::Specifier::KwWhile:
		return parse_while_loop_stmt();
	case Token::Specifier::KwIf:
		return parse_if_stmt();
	case Token::Specifier::Ident:
	{
		if (la2 == Token::Specifier::Asterix // e.g. int*
			|| la2 == Token::Specifier::Less // e.g. List<
			|| la2 == Token::Specifier::BracketL // e.g. int[
			|| la2 == Token::Specifier::DeclCpy // e.g. a :=
			|| la2 == Token::Specifier::DeclMv // e.g. a :#
			|| la2 == Token::Specifier::Colon // e.g. a :
			|| la3 == Token::Specifier::DeclCpy // int a :=
			|| la3 == Token::Specifier::DeclMv // int b :#
			|| la3 == Token::Specifier::Colon)  // int c : 
			return parse_decl_operator_stmt(); // Also parses decl
		else if (la2 == Token::Specifier::Ident) // int a;
			return parse_decl_stmt();
	}
	break;
	case Token::Specifier::KwFptr:
		return parse_decl_stmt();
	default:
		break;
	}
	return parse_expr_stmt();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_allowed_func_stmts()
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		stmts.push_back(parse_allowed_func_stmt());
	}
	return stmts;
}

std::unique_ptr<Stmt> Parser::parse_expr_stmt()
{
	auto out = std::make_unique<ExprStmt>(expr_parser.parse());
	tkns.match_token(Token::Specifier::Semicolon);
	return out;
}

std::unique_ptr<Stmt> Parser::parse_return_stmt()
{
	auto& rk = tkns.match_token(Token::Specifier::KwReturn);
	auto out = std::make_unique<ReturnStmt>(expr_parser.parse(),mv(rk));
	tkns.match_token(Token::Specifier::Semicolon);
	return out;
}
