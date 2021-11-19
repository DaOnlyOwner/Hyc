#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"
#include "DebugPrint.h"
#include <cstdlib>
#include "Ast.h"
#include <memory>


// Expressions

// Infix Operators
#define InfixExprFnArgs ExprParser& parser, const Token& token, std::unique_ptr<Expr> lh
#define PrefixExprFnArgs ExprParser& parser, const Token& token


namespace
{
	// Precedence  Right assoc?  Parsing function
	InfixOperation<Expr> productGroup{ (int)ExprPrecedence::productGroup, false, [](InfixExprFnArgs) {
		return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal((int)ExprPrecedence::productGroup));
	} };

	InfixOperation<Expr> sumGroup{ (int)ExprPrecedence::sumGroup, false, [](InfixExprFnArgs) {
		return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal((int)ExprPrecedence::sumGroup));
	} };


	// "Prefix" Operators

	PrefixOperation<Expr> float_lit{ (int)ExprPrecedence::float_lit, [](PrefixExprFnArgs)
	{
		return ast_as<Expr>(std::make_unique<FloatLiteralExpr>(token));
	} };

	std::pair<bool,EvalIntegerResult> eval_integer_val(const Token& token)
	{
		const std::string& integer_text = token.text;
		errno = 0;
		if (token.text[0] == '-')
		{
			auto val = strtoll(token.text.c_str(), nullptr, 0);
			if (errno != 0)
			{
				return { false,{Primitive::Specifier::Count,0,0 } };
			}
			return { true,{Primitive::Specifier::int_,0,val} };
		}

		else
		{
			errno = 0;
			auto val= strtoull(token.text.c_str(), nullptr, 0);
			if (errno != 0)
			{
				return { false,{Primitive::Specifier::Count,0,0 } };
			}
			return { true,{Primitive::Specifier::uint,val,0 } };
		}
	}

	PrefixOperation<Expr> integer_lit{ (int)ExprPrecedence::integer_lit, [](PrefixExprFnArgs)
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

	PrefixOperation<Expr> ident_expr{ (int)ExprPrecedence::ident_expr, [](PrefixExprFnArgs) {
		// Function call
		Lexer& l = parser.overall_parser->get_lexer();
		if (l.lookahead(1).type == Token::Specifier::RParenL || l.lookahead(1).type == Token::Specifier::Less)
		{
			std::vector<uptr<TypeSpec>> generic_params;
			// Parse generic parameters
			if (l.lookahead(1).type == Token::Specifier::Less)
			{
				l.eat(); // <
				auto fst = parser.overall_parser->parse_type_spec();
				while (l.lookahead(1).type == Token::Specifier::Comma)
				{
					l.eat(); // ,
					generic_params.push_back(parser.overall_parser->parse_type_spec());
				}
				l.match_token(Token::Specifier::Greater); // >
			}
			l.eat(); // '('
			std::vector<uptr<Expr>> arg_list;
			// Parse an argument list:
			while (l.lookahead(1).type != Token::Specifier::RParenR && l.lookahead(1).type != Token::Specifier::Eof)
			{
				arg_list.push_back(parser.parse());
				if (l.lookahead(1).type == Token::Specifier::Comma)
					l.eat();
			}

			l.match_token(Token::Specifier::RParenR); // ')'

			return static_cast<uptr<Expr>>(std::make_unique<FuncCallExpr>(Token(token), mv(generic_params), mv(arg_list)));
		}
		// Just an identifier.
		return static_cast<uptr<Expr>>(std::make_unique<IdentExpr>(token));
	} };

	PrefixOperation<Expr> unary_operator{ (int)ExprPrecedence::unary_op, [](PrefixExprFnArgs)
	{
		return ast_as<Expr>(std::make_unique<PrefixOpExpr>(token, parser.parse_internal((int)ExprPrecedence::unary_op)));
	} };
}

Parser::Parser(Lexer& token_source, const std::string& filename)
	:expr_parser(this,token_source),
	tkns(token_source)
{
	expr_parser.add_operation(Token::Specifier::Asterix, productGroup);
	expr_parser.add_operation(Token::Specifier::Plus, sumGroup);
	expr_parser.add_operation(Token::Specifier::Float, float_lit);
	expr_parser.add_operation(Token::Specifier::Integer, integer_lit);
	expr_parser.add_operation(Token::Specifier::Minus, unary_operator);
	expr_parser.add_operation(Token::Specifier::Plus, unary_operator);
	expr_parser.add_operation(Token::Specifier::Ident, ident_expr);

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

	std::vector<uptr<Stmt>> body;
	
	tkns.match_token(Token::Specifier::BraceL); // '{'
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		body.push_back(parse_allowed_func_stmt());
	}

	tkns.match_token(Token::Specifier::BraceR); // '}'
	func_decl->body = mv(body);
	return func_decl;
}

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
	if (tkns.lookahead(1).type == Token::Specifier::Ident)
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
		auto inner = parse_type_spec_part();
		return std::make_unique<BaseTypeSpec>(name, mv(inner), std::move(generics));
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

GenericInfo Parser::parse_generic_param()
{
	GenericInfo gi;
	gi.name = mv(tkns.match_token(Token::Specifier::Ident));
	if (tkns.lookahead(1).type == Token::Specifier::Colon)
	{
		tkns.eat(); // :
		gi.needed_contracts.push_back(tkns.match_token(Token::Specifier::Ident)); // ident
		while (tkns.lookahead(1).type == Token::Specifier::Plus)
		{
			tkns.eat(); // +
			gi.needed_contracts.push_back(std::move(tkns.match_token(Token::Specifier::Ident))); // Do I need to std::move here?
		}
	}
	if (tkns.lookahead(1).type == Token::Specifier::Equal) 
	{
		tkns.eat(); // =
		gi.default_type = parse_type_spec();
	}
	return gi;
}

// ident ( : ident "+" (ident)* )?
// T : a + b, C:a
std::vector<GenericInfo> Parser::parse_generic_list()
{
	std::vector<GenericInfo> gi_lst;
	GenericInfo fst = parse_generic_param();
	while (tkns.lookahead(1).type == Token::Specifier::Comma)
	{
		tkns.eat();
		gi_lst.push_back(parse_generic_param());
	}
	return gi_lst;
}

// type ident ;
std::unique_ptr<Stmt> Parser::parse_decl_stmt()
{
	auto type = parse_type_spec();
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<DeclStmt>(mv(type), mv(name));
}


// ident ("=" ident)? (","(ident ("=" ident)? ",")*)?
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
	tkns.match_token(Token::Specifier::KwStruct);
	auto& name = tkns.match_token(Token::Specifier::Ident);
	std::vector<GenericInfo> generic_parameters;
	if (tkns.lookahead(1).type == Token::Specifier::Less)
	{
		tkns.eat();
		generic_parameters = parse_comma_separated_ident_list();
		tkns.match_token(Token::Specifier::Greater);
	}
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> decls_inside_struct;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		decls_inside_struct.push_back(parse_decl_stmt());
	}
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<StructDefStmt>(mv(name), mv(generic_parameters), mv(decls_inside_struct));
}

// namespace ident { allowed_namespace_stmts }
std::unique_ptr<Stmt> Parser::parse_namespace_stmt()
{
	tkns.match_token(Token::Specifier::KwNamespace);
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> stmts = parse_allowed_namespace_stmts();
	return std::make_unique<NamespaceStmt>(mv(stmts),mv(name));
	tkns.match_token(Token::Specifier::BraceR);
}


std::unique_ptr<Stmt> Parser::parse_allowed_namespace_stmt()
{
	if (tkns.lookahead(3).type == Token::Specifier::Semicolon)
		return parse_decl_stmt();
	else if (tkns.lookahead(1).type == Token::Specifier::KwStruct)
		return parse_struct_def();
	else if (tkns.lookahead(1).type == Token::Specifier::KwNamespace)
		return parse_namespace_stmt();
	else if (tkns.lookahead(1).type == Token::Specifier::KwContract)
		return parse_contract_def();
	else if (tkns.lookahead(1).type == Token::Specifier::KwFulfill)
		return parse_contract_impl_block();
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

// contract ident (: ident ("+" (ident "+" )*)? ) { allowed_contract_stmts }
std::unique_ptr<Stmt> Parser::parse_contract_def()
{
	tkns.match_token(Token::Specifier::KwContract);
	auto& name = tkns.match_token(Token::Specifier::Ident);
	std::vector<Token> inherited_contracts;
	if (tkns.lookahead(1).type == Token::Specifier::Colon)
	{
		tkns.eat(); // :
		auto& inherited = tkns.match_token(Token::Specifier::Ident);
		inherited_contracts.push_back(mv(inherited));
		while (tkns.lookahead(1).type == Token::Specifier::Plus)
		{
			tkns.eat(); // +
			auto& inherited_further = tkns.match_token(Token::Specifier::Ident);
			inherited_contracts.push_back(mv(inherited_further));
		}
	}
	tkns.match_token(Token::Specifier::BraceL);
	auto stmts = parse_allowed_contract_stmts();
	return std::make_unique<ContractDefStmt>(mv(name), mv(inherited_contracts), mv(stmts));
}

// (function_def | function_decl)*
std::vector<std::unique_ptr<Stmt>> Parser::parse_allowed_contract_stmts()
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		auto decl = static_cast<uptr<Stmt>>(parse_function_decl_stmt_part());
		if (tkns.lookahead(1).type == Token::Specifier::Semicolon)
		{
			tkns.eat();
			stmts.push_back(mv(decl));
		}
		else if(tkns.lookahead(1).type == Token::Specifier::BraceL)
		{
			stmts.push_back(parse_function_def_stmt());
		}
		else
		{
			auto descr = Error::FromToken(tkns.lookahead(1));
			descr.Message = fmt::format("Expected a function declaration or function definition, but got {}", tkns.lookahead(1).text);
			Error::SyntacticalError(descr);
		}
	}
	tkns.match_token(Token::Specifier::BraceR);
	return stmts;
}

std::unique_ptr<FuncDefStmt> Parser::parse_function_decl_stmt_part()
{
	auto type = parse_type_spec(); // Type
	auto& name = tkns.match_token(Token::Specifier::Ident); // ident

	std::vector<GenericInfo> generic_list;
	if (tkns.lookahead(1).type == Token::Specifier::Less) // <
	{
		tkns.eat();
		generic_list = parse_generic_list();
		tkns.match_token(Token::Specifier::Greater);
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
	return std::make_unique<FuncDefStmt>(mv(type), mv(name), mv(generic_list), mv(param_list), std::vector<uptr<Stmt>>{});
}

std::unique_ptr<Stmt> Parser::parse_function_decl_stmt()
{
	auto out = parse_function_decl_stmt_part();
	tkns.match_token(Token::Specifier::Semicolon);
	return out;
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

// fullfill ident for ident { func_def_list }
std::unique_ptr<Stmt> Parser::parse_contract_impl_block()
{
	tkns.match_token(Token::Specifier::KwFulfill);
	auto& contr = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::KwFor);
	auto& for_name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::BraceL);
	auto def_list = parse_func_def_list();
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<ContractImplStmt>(mv(contr), mv(for_name), mv(def_list));
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
