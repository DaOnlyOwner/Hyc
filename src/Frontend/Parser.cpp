#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"
#include "DebugPrint.h"
#include <cstdlib>
#include "Ast.h"
#include <memory>
#include "Messages.h"
#include <map>

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

#define DEF_INTEGER_LIT(name, type)\
PrefixOperation<Expr> name{ (int)ExprPrecedence::Group0, [](PrefixExprFnArgs)\
	{\
			auto [succ,eval_res] = eval_integer_val(token,IntegerLiteralType::type);\
			if (!succ)\
			{\
				auto descr = Error::FromToken(token);\
				descr.Message = fmt::format("{} doesn't fit into an 64 bit integer", token.text);\
				Error::SyntacticalError(descr);\
			}\
		return ast_as<Expr>(std::make_unique<IntegerLiteralExpr>(token,eval_res,IntegerLiteralType::type));\
	} };

#define DEF_DECIMAL_LIT(name,type,text_)\
PrefixOperation<Expr> name{ (int)ExprPrecedence::Group0, [](PrefixExprFnArgs)\
		{\
			auto [succ,decimalVal] = eval_decimal_val(token,DecimalLiteralType::type);\
			if (!succ)\
			{\
				auto descr = Error::FromToken(token);\
				descr.Message = fmt::format(("{} doesn't fit into a" text_), token.text);\
				Error::SyntacticalError(descr);\
			}\
			return ast_as<Expr>(std::make_unique<DecimalLiteralExpr>(token,DecimalLiteralType::type,decimalVal));\
		}}

#define DEF_MEMOP(name)\
InfixOperation<Expr> name{ (int)ExprPrecedence::Group16, true, [](InfixExprFnArgs) {\
		auto snd = parser.parse();\
		parser.overall_parser->get_lexer().match_token(Token::Specifier::Colon);\
		auto trd = parser.parse_internal((int)ExprPrecedence::Group16);\
		return std::make_unique<MemOpExpr>(mv(lh), mv(snd), mv(trd),token);\
	} };

#define ADD_OP(op,name) expr_parser.add_operation(op,name)

namespace
{

	std::pair<bool, EvalIntegerResult> eval_integer_val(const Token& token, IntegerLiteralType ilt)
	{
		static int map_to_bitwidth[(int)IntegerLiteralType::Count] = { 64,32,16,8,64,32,16,8 };
		static bool map_to_signed[(int)IntegerLiteralType::Count] = { false,false,false,false,true,true,true,true };
		std::string integer_text = token.text;
		if (integer_text.back() == 'i'
			|| integer_text.back() == 'h'
			|| integer_text.back() == 's'
			|| integer_text.back() == 'c')
			integer_text.pop_back();

		else if (ilt == IntegerLiteralType::UInt || ilt == IntegerLiteralType::UChar || ilt==IntegerLiteralType::UHalf)
		{
			integer_text.pop_back();
			integer_text.pop_back();
		}

		errno = 0;
		auto val = strtoull(integer_text.c_str(), nullptr, 0);
		int amount_bits = map_to_bitwidth[(int)ilt];
		bool is_signed = map_to_signed[(int)ilt];
		if (errno != 0)
		{
			return { false,{0,0,0} };
		}
		return { true,{amount_bits,val,is_signed} };
	}

	std::pair<bool, std::variant<float, double>> eval_decimal_val(const Token& tk, DecimalLiteralType dlt)
	{
		std::string dec_text = tk.text;
		if (dec_text.back() == 'f'
			|| dec_text.back() == 'd'
			|| dec_text.back() == 'q')
			dec_text.pop_back();
		errno = 0;
		std::variant<float, double> holder;
		if (dlt == DecimalLiteralType::Float)
		{
			holder = strtof(dec_text.c_str(), nullptr);
		}
		else if (dlt == DecimalLiteralType::Double)
		{
			holder = strtod(dec_text.c_str(), nullptr);
		}
		if (errno != 0)
		{
			return { false,{} };
		}
		return { true,holder };
	}

	// TODO: Add del operator
	DEF_INTEGER_LIT(uint_lit, UInt);
	DEF_INTEGER_LIT(uhalf_lit, UHalf);
	DEF_INTEGER_LIT(ushort_lit, UShort);
	DEF_INTEGER_LIT(uchar_lit, UChar);
	DEF_INTEGER_LIT(int_lit, Int);
	DEF_INTEGER_LIT(half_lit, Half);
	DEF_INTEGER_LIT(short_lit, Short);
	DEF_INTEGER_LIT(char_lit, Char);

	DEF_DECIMAL_LIT(float_lit, Float, "float");
	DEF_DECIMAL_LIT(double_lit, Double, "double");


	PrefixOperation<Expr> quad_lit{ (int)ExprPrecedence::Group0, [](PrefixExprFnArgs) -> uptr<Expr>
		{
			NOT_IMPLEMENTED;
		}
	};

	PrefixOperation<Expr> ident{ (int)ExprPrecedence::Group0, [](PrefixExprFnArgs)
	{
			auto& l = parser.overall_parser->get_lexer();
			std::vector<uptr<TypeSpec>> generic_params;
			if (l.lookahead(1).type == Token::Specifier::GenFCallOpen)
			{
				l.eat(); // <.
				auto fst = parser.overall_parser->parse_type_spec();
				if(fst) generic_params.push_back(mv(fst));
				while (l.lookahead(1).type == Token::Specifier::Comma)
				{
					l.eat(); // ,
					generic_params.push_back(parser.overall_parser->parse_type_spec());
				}
				l.match_token(Token::Specifier::GenFCallClose); // .>
			}
			if (l.lookahead(1).type == Token::Specifier::RParenL && l.lookahead(2).type == Token::Specifier::Semicolon)
			{
				std::vector<uptr<TypeSpec>> params;
				l.eat(); // (
				l.eat(); // ;
				auto fst = parser.overall_parser->parse_type_spec();
				if (fst) params.push_back(mv(fst));
				while (l.lookahead(1).type == Token::Specifier::Comma)
				{
					l.eat(); // ,
					params.push_back(parser.overall_parser->parse_type_spec());
				}
				l.match_token(Token::Specifier::RParenR);
				return static_cast<std::unique_ptr<Expr>>(std::make_unique<FptrIdentExpr>(token, mv(generic_params), mv(params)));
			}
			return static_cast<std::unique_ptr<Expr>>(std::make_unique<IdentExpr>(token,mv(generic_params)));
	} };
	PrefixOperation<Expr> paren{(int)ExprPrecedence::Group0,[](PrefixExprFnArgs){
		auto expr = parser.parse();
		parser.overall_parser->get_lexer().match_token(Token::Specifier::RParenR);
		return std::move(expr);
	}};
	// Precedence  right_assoc?  Parsing function
	DEF_BIN_OP(scope, Group1, false);
	DEF_POSTFIX_OP(incr_decr, Group2, false);
	InfixOperation<Expr> func_call_array_sub{(int)ExprPrecedence::Group2, false, [](InfixExprFnArgs) {
		// Function call
		Lexer& l = parser.overall_parser->get_lexer();
		if(token.type == Token::Specifier::RParenL)
		{
			std::vector<FuncCallArg> arg_list;
			// Parse an argument list:
			while (l.lookahead(1).type != Token::Specifier::RParenR && l.lookahead(1).type != Token::Specifier::Eof)
			{
				FuncCallArg arg;
				arg.expr = parser.parse();
				arg_list.push_back(mv(arg));
				if (l.lookahead(1).type == Token::Specifier::Comma)
					l.eat();
			}

			l.match_token(Token::Specifier::RParenR); // ')'

			return static_cast<uptr<Expr>>(std::make_unique<FuncCallExpr>(mv(lh), mv(arg_list)));
		}

		else if (token.type == Token::Specifier::BracketL)
		{
			auto expr = parser.parse();
			l.match_token(Token::Specifier::BracketR);
			return static_cast<uptr<Expr>>(std::make_unique<ArraySubscriptExpr>(mv(lh), mv(expr)));
		}
		else
		{
			assert(false);
			return uptr<Expr>(nullptr);
		}
	} };
	DEF_BIN_OP(member, Group2, false);
	DEF_BIN_OP(make_active, Group2, false);
	DEF_BIN_OP(test_if_active, Group2, false);
	DEF_PREFIX_OP(unary, Group3);
	
	PrefixOperation<Expr> offsetof_{(int)ExprPrecedence::Group2,[](PrefixExprFnArgs){
		auto& l = parser.overall_parser->get_lexer();
		l.match_token(Token::Specifier::RParenL);
		auto of = parser.overall_parser->parse_base_type_spec();
		l.match_token(Token::Specifier::Comma);
		auto member = l.match_token(Token::Specifier::Ident);
		l.match_token(Token::Specifier::RParenR);
		return std::make_unique<OffsetofExpr>(token,std::move(of),member);
	}};

	PrefixOperation<Expr> align_sizeof{(int)ExprPrecedence::Group3,[](PrefixExprFnArgs){
		auto expr = parser.parse_internal((int)ExprPrecedence::Group3);
		return std::make_unique<SizeOrAlignmentInfoExpr>(token,mv(expr));
	}};
	// TODO: Cast
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

	InfixOperation<Expr> mem_op{ (int)ExprPrecedence::Group16, true, [](InfixExprFnArgs) {
		auto snd = parser.parse();
		parser.overall_parser->get_lexer().match_token(Token::Specifier::Colon);
		auto trd = parser.parse_internal((int)ExprPrecedence::Group16);
		return std::make_unique<MemOpExpr>(mv(lh), mv(snd), mv(trd),token);
	} };

	InfixOperation<Expr> if_expr{ (int)ExprPrecedence::Group16, true, [](InfixExprFnArgs) {
		auto snd = parser.parse();
		parser.overall_parser->get_lexer().match_token(Token::Specifier::Colon);
		auto trd = parser.parse_internal((int)ExprPrecedence::Group16);
		return std::make_unique<TernaryExpr>(mv(lh), mv(snd), mv(trd));
	} };

	DEF_BIN_OP(assignment_etc, Group16, true);
	DEF_PREFIX_OP(throw_, Group16);	
}

Parser::Parser(Lexer& token_source, const std::string& filename)
	:expr_parser(this,token_source),
	tkns(token_source)
{
	//expr_parser.add_operation(Token::Specifier::DoubleColon, scope);
	expr_parser.add_operation(Token::Specifier::DoublePlus, incr_decr);
	ADD_OP(Token::Specifier::DoubleMinus, incr_decr);
	ADD_OP(Token::Specifier::RParenL, func_call_array_sub);
	ADD_OP(Token::Specifier::BracketL, func_call_array_sub);
	ADD_OP(Token::Specifier::Dot, member);
	ADD_OP(Token::Specifier::MemAccess, member);
	ADD_OP(Token::Specifier::DotComma, member);
	ADD_OP(Token::Specifier::MemAccComma, member);
	ADD_OP(Token::Specifier::Minus, unary);
	ADD_OP(Token::Specifier::Plus, unary);
	ADD_OP(Token::Specifier::ExclMark, unary);
	ADD_OP(Token::Specifier::Tilde, unary);
	//ADD_OP(Token::Specifier::KwAs, cast);
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
	ADD_OP(Token::Specifier::Equal, assignment_etc);
	ADD_OP(Token::Specifier::Hashtag, assignment_etc);
	ADD_OP(Token::Specifier::PlusEqual, assignment_etc);
	ADD_OP(Token::Specifier::MinusEqual, assignment_etc);
	ADD_OP(Token::Specifier::AsterixEqual, assignment_etc);
	ADD_OP(Token::Specifier::SlashEqual, assignment_etc);
	ADD_OP(Token::Specifier::SlEqual, assignment_etc);
	ADD_OP(Token::Specifier::SrEqual, assignment_etc);
	ADD_OP(Token::Specifier::PercentEqual, assignment_etc);
	ADD_OP(Token::Specifier::AmpersandEqual, assignment_etc);
	ADD_OP(Token::Specifier::CaretEqual, assignment_etc);
	ADD_OP(Token::Specifier::OrEqual, assignment_etc);
	// TODO: Add compound operators for move #/, #+ etc.
	//ADD_OP(Token::Specifier::KwThrow, throw_);
	ADD_OP(Token::Specifier::Ident, ident);
	ADD_OP(Token::Specifier::Int, int_lit);
	ADD_OP(Token::Specifier::Half, half_lit);
	ADD_OP(Token::Specifier::Short, short_lit);
	ADD_OP(Token::Specifier::Char, char_lit);
	ADD_OP(Token::Specifier::UInt, uint_lit);
	ADD_OP(Token::Specifier::UHalf, uhalf_lit);
	ADD_OP(Token::Specifier::UShort, ushort_lit);
	ADD_OP(Token::Specifier::UChar, uchar_lit);
	ADD_OP(Token::Specifier::Double, double_lit);
	ADD_OP(Token::Specifier::Quad, quad_lit);
	ADD_OP(Token::Specifier::DoubleQM, test_if_active);
	ADD_OP(Token::Specifier::DoubleEM, make_active);
	ADD_OP(Token::Specifier::MemCpy, mem_op);
	ADD_OP(Token::Specifier::MemMove, mem_op);
	ADD_OP(Token::Specifier::MemSet, mem_op);
	ADD_OP(Token::Specifier::RParenL,paren);
	ADD_OP(Token::Specifier::KwAlignof,align_sizeof);
	ADD_OP(Token::Specifier::KwSizeof,align_sizeof);
	ADD_OP(Token::Specifier::KwOffsetof,offsetof_);

	file = filename;
}

std::unique_ptr<NamespaceStmt> Parser::parse()
{
	return parse_compilation_unit();
}

// stmt*
std::unique_ptr<NamespaceStmt> Parser::parse_compilation_unit()
{
	auto nms = std::make_unique<NamespaceStmt>(Token(Token::Specifier::Ident, "GLOBAL", file, "", 0, 0, 0, 0));
	while (tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		nms->stmts.push_back(parse_allowed_top_level_stmt());
	}
	return mv(nms);
}

std::unique_ptr<Stmt> Parser::parse_operator_def_stmt()
{
	tkns.match_token(Token::Specifier::KwOperator);
	auto type = parse_type_spec();
	auto op = tkns.match_one_of<
		Token::Specifier::KwDel,
		Token::Specifier::Equal,
		Token::Specifier::Hashtag
	>();
	
	std::vector<FuncArg> param_list;

	tkns.match_token(Token::Specifier::RParenL);

	while (!(tkns.lookahead(1).type == Token::Specifier::RParenR || tkns.lookahead(1).type == Token::Specifier::Eof))
	{
		auto arg_type = parse_type_spec(); // Type
		auto& arg_name = tkns.match_token(Token::Specifier::Ident); // name

		param_list.push_back({ false,std::make_unique<DeclStmt>(std::move(arg_type), arg_name) });

		if (tkns.lookahead(1).type == Token::Specifier::Comma) tkns.eat();
	}
	tkns.match_token(Token::Specifier::RParenR);

	auto decl = std::make_unique<FuncDeclStmt>(std::move(type), op, nullptr, std::move(param_list));
	auto body = parse_function_body();
	return std::make_unique<FuncDefStmt>(std::move(decl), std::move(body),true);
}

// (type ident ((:= | :) expr)?) | (type? ident (:= | :) expr)  ; 
std::unique_ptr<Stmt> Parser::parse_decl_operator_stmt()
{
	auto la2 = tkns.lookahead(2).type;
	std::unique_ptr<TypeSpec> type = nullptr;
	if (la2 != Token::Specifier::DeclCpy && la2 != Token::Specifier::Colon && la2 != Token::Specifier::Equal)
		type = parse_type_spec(); // type
	auto& ident = tkns.match_token(Token::Specifier::Ident); // Ident
	auto la1 = tkns.lookahead(1).type;
	std::unique_ptr<Expr> expr;

	//assert(!(type == nullptr && la1 == Token::Specifier::Equal));
	//			^--- This should only happen when we are in simulation mode
	if (type == nullptr && la1 == Token::Specifier::Equal)
	{
		assert(!Error::DoEmit);
		throw Error::SyntaxErrorException(); // Bail out of simulation mode.
	}

	if (la1 == Token::Specifier::DeclCpy || la1 == Token::Specifier::Equal)
	{
		tkns.eat(); // := || =
		expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::Semicolon); // ;
		return std::make_unique<DeclCpyStmt>(std::move(type), ident, std::move(expr));
	}

	// TODO: DeclMove 
	else if (la1 == Token::Specifier::Colon)
	{
		tkns.eat(); // :
		expr = expr_parser.parse();
		tkns.match_token(Token::Specifier::Semicolon); // ;
		return std::make_unique<DeclInitStmt>(std::move(type), ident, std::move(expr));
	}
	// Is a declaration stmt
	else if (la1 == Token::Specifier::Semicolon)
	{
		assert(type != nullptr); // It would look like this "a;" which is an expression. Contrary to the grammar spec we should never step into this case in this function
		tkns.match_token(Token::Specifier::Semicolon); // ; 
		return std::make_unique<DeclStmt>(std::move(type), ident);
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
		static std::map<Token::Specifier, IntegerLiteralType> specToIT = {
			{Token::Specifier::UInt,IntegerLiteralType::UInt},
			{Token::Specifier::Int,IntegerLiteralType::Int},
			{Token::Specifier::Half,IntegerLiteralType::Half},
			{Token::Specifier::UHalf,IntegerLiteralType::UHalf},
			{Token::Specifier::Short,IntegerLiteralType::UShort},
			{Token::Specifier::Char, IntegerLiteralType::UChar}
		};
		Token integer = tkns.match_one_of<
			Token::Specifier::UInt,
			Token::Specifier::Int,
			Token::Specifier::Half, 
			Token::Specifier::UHalf,
			Token::Specifier::Char,
			Token::Specifier::UChar,
			Token::Specifier::Short,
			Token::Specifier::UShort>(); // integer
		tkns.match_token(Token::Specifier::BracketR); // ]
		auto it = specToIT[integer.type];
		auto [succ, res] = eval_integer_val(integer,it);
		if (!succ)
		{
			auto descr = Error::FromToken(integer);
			descr.Message = fmt::format("{} doesn't fit into an 64 bit integer", integer.text);
			Error::SyntacticalError(descr);
		}

		auto integerLiteral = std::make_unique<IntegerLiteralExpr>(integer, res,it);
		return std::make_unique<ArrayTypeSpec>(std::move(integerLiteral), parse_type_spec_part());
	}
	else return nullptr;
}

// type_spec := (ident(< ident* >)? | fptr(type_spec;type_spec*) | "*" | "[integer_literal]") type_spec
std::unique_ptr<TypeSpec> Parser::parse_type_spec()
{
	if (tkns.lookahead(1).type == Token::Specifier::Ident)
	{
		return parse_base_type_spec();
	}

	else if (tkns.lookahead(1).type == Token::Specifier::KwAuto)
	{
		tkns.eat(); // auto;
		return nullptr;
	}

	else if (tkns.lookahead(1).type == Token::Specifier::KwFptr)
	{
		tkns.eat();
		tkns.match_token(Token::Specifier::RParenL);
		std::vector<uptr<TypeSpec>> args;
		auto ret_type = parse_type_spec();
		tkns.match_token(Token::Specifier::Semicolon);
		while (tkns.lookahead(1).type != Token::Specifier::RParenR && tkns.lookahead(1).type != Token::Specifier::Eof)
		{
			args.push_back(parse_type_spec()); 
			if (tkns.lookahead(1).type == Token::Specifier::Comma)
				tkns.eat();
			//else if (tkns.lookahead(1).type != Token::Specifier::RParenR) break;
		}
		tkns.match_token(Token::Specifier::RParenR);
		auto inner = parse_type_spec_part();
		return std::make_unique<FptrTypeSpec>(mv(args), mv(ret_type),mv(inner));
	}
	return nullptr;
}

inline std::unique_ptr<BaseTypeSpec> Parser::parse_base_type_spec()
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

// type ident ;
std::unique_ptr<Stmt> Parser::parse_decl_stmt()
{
	auto type = parse_type_spec();
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<DeclStmt>(mv(type), name);
}

std::unique_ptr<InitList> Parser::parse_init_list()
{
	tkns.match_token(Token::Specifier::BraceL);

}

std::unique_ptr<Stmt> Parser::parse_union_decl_stmt()
{
	uptr<DeclStmt> decl_stmt;
	if (tkns.lookahead(2).type == Token::Specifier::Semicolon || tkns.lookahead(2).type == Token::Specifier::Colon)
	{
		// Untyped:
		auto name = tkns.match_token(Token::Specifier::Ident);
		decl_stmt = std::make_unique<DeclStmt>(nullptr, name);
	}

	else
	{
		auto type_spec = parse_type_spec();
		auto name = tkns.match_token(Token::Specifier::Ident);
		decl_stmt = std::make_unique<DeclStmt>(std::move(type_spec), name);
	}

	std::optional<EvalIntegerResult> tagged_value{};

	if (tkns.lookahead(1).type == Token::Specifier::Colon)
	{
		tkns.eat(); // :
		auto val = tkns.match_token(Token::Specifier::Int);
		auto [succ,res] = eval_integer_val(val, IntegerLiteralType::Int);
		if (!succ)
		{
			auto descr = Error::FromToken(val);
			descr.Message = fmt::format("{} doesn't fit into an 64 bit integer", val.text);
			Error::SyntacticalError(descr);
		}
		tagged_value = res;
	}
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<UnionDeclStmt>(mv(decl_stmt), tagged_value);
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

void Parser::save()
{
	savepoints.push_back(tkns.get_tk_pos());
}

void Parser::backtrack()
{
	auto tp = savepoints.back();
	savepoints.pop_back();
	tkns.goto_tk_pos(tp);
}

// struct ident< comma_separated_ident_list > { declaration_stmt* }
std::unique_ptr<Stmt> Parser::parse_struct_def()
{
	return parse_attr_collection(Token::Specifier::KwStruct, [&]() {return parse_decl_stmt(); });
}

std::unique_ptr<Stmt> Parser::parse_union_def()
{
	return parse_attr_collection(Token::Specifier::KwUnion, [&]() {return parse_union_decl_stmt(); });
}

// namespace ident { allowed_namespace_stmts }
std::unique_ptr<Stmt> Parser::parse_namespace_stmt()
{
	tkns.match_token(Token::Specifier::KwNamespace);
	auto& name = tkns.match_token(Token::Specifier::Ident);
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> stmts = parse_allowed_namespace_stmts();
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<NamespaceStmt>(mv(stmts),name);
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
	else return parse_decl_stmt();
	auto descr = Error::FromToken(tkns.lookahead(1));
	descr.Message = "Expected a declaration statement, a struct or union definition statement or a namespace definition statement";
	descr.Hint = "Function definitions are not allowed inside those statements, they need to be top level definitions";
	Error::SyntacticalError(descr);
}

std::unique_ptr<Stmt> Parser::parse_allowed_top_level_stmt()
{
	//if (tkns.lookahead(3).type == Token::Specifier::Semicolon)
		//return parse_decl_stmt();
	if (tkns.lookahead(1).type == Token::Specifier::KwStruct)
		return parse_struct_def();
	else if (tkns.lookahead(1).type == Token::Specifier::KwUnion)
		return parse_union_def();
	else if (tkns.lookahead(1).type == Token::Specifier::KwOperator)
		return parse_operator_def_stmt();
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
	bool ret_moved = false;
	if (tkns.lookahead(1).type == Token::Specifier::KwMoved)
	{
		ret_moved = true;
		tkns.eat();
	}
	auto type = parse_type_spec(); // Type

	Token name;
	std::vector<GenericInfo> generic_list;
	uptr<DeclStmt> named_return = nullptr;

	if (tkns.lookahead(1).type == Token::Specifier::Ident && tkns.lookahead(2).type == Token::Specifier::Ident)
	{
		auto nr = tkns.eat(); // Ident
		name = tkns.eat(); // Ident
		auto cloned_type = type->clone();
		cloned_type->push_inner(std::make_unique<PointerTypeSpec>(nullptr));
		named_return = std::make_unique<DeclStmt>(mv(cloned_type), nr);
	}
	else name = tkns.match_token(Token::Specifier::Ident); // ident

	if (tkns.lookahead(1).type == Token::Specifier::Less) // <
	{
		tkns.eat(); // <
		generic_list = parse_comma_separated_ident_list();
		tkns.match_token(Token::Specifier::Greater);
	}

	std::vector<FuncArg> param_list;

	tkns.match_token(Token::Specifier::RParenL);

	while (!(tkns.lookahead(1).type == Token::Specifier::RParenR || tkns.lookahead(1).type == Token::Specifier::Eof))
	{
		bool moved = false;
		if (tkns.lookahead(1).type == Token::Specifier::KwMoved)
		{
			moved = true;
			tkns.eat(); // moved
		}
		auto arg_type = parse_type_spec(); // Type
		auto& arg_name = tkns.match_token(Token::Specifier::Ident); // name

		param_list.push_back({ moved, std::make_unique<DeclStmt>(std::move(arg_type), arg_name) });

		if (tkns.lookahead(1).type == Token::Specifier::Comma) tkns.eat();
	}

	tkns.match_token(Token::Specifier::RParenR); // ')'

	return std::make_unique<FuncDeclStmt>(mv(type), name, mv(named_return), mv(generic_list), mv(param_list),ret_moved);
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
	auto var = tkns.match_token(Token::Specifier::Ident);
	std::optional<Token> as;
	if (tkns.lookahead(1).type == Token::Specifier::KwAs)
	{
		tkns.eat(); // as
		as = tkns.match_token(Token::Specifier::Ident);
	}
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> body = parse_allowed_func_stmts(in_loop);
	tkns.match_token(Token::Specifier::BraceR);
	return MatchCase(var,as, mv(body));
}

std::unique_ptr<Stmt> Parser::parse_continue_stmt()
{
	tkns.match_token(Token::Specifier::KwContinue);
	tkns.match_token(Token::Specifier::Semicolon);
	return std::make_unique<ContinueStmt>();
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
std::unique_ptr<Stmt> Parser::parse_if_stmt(bool in_loop)
{
	tkns.match_token(Token::Specifier::KwIf);
	tkns.match_token(Token::Specifier::RParenL);
	auto if_expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR);
	std::vector<uptr<Stmt>> if_stmts;
	if (tkns.lookahead(1).type == Token::Specifier::BraceL)
	{
		tkns.eat();
		if_stmts = parse_allowed_func_stmts(in_loop);
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
			elif_stmts = parse_allowed_func_stmts(in_loop);
			tkns.match_token(Token::Specifier::BraceR);
		}
		// TODO: Rework
		else if (tkns.lookahead(1).type != Token::Specifier::KwElif && tkns.lookahead(1).type != Token::Specifier::KwElse)
		{
			elif_stmts.push_back(parse_allowed_func_stmt(in_loop));
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
			else_stmts = parse_allowed_func_stmts(in_loop);
			tkns.match_token(Token::Specifier::BraceR);
		}
		else if (tkns.lookahead(1).type != Token::Specifier::KwElif && tkns.lookahead(1).type != Token::Specifier::KwElse)
		{
			else_stmts.push_back(parse_allowed_func_stmt(in_loop));
		}
	}
	
	return std::make_unique<IfStmt>(mv(if_expr), mv(if_stmts), mv(elif_exprs), mv(all_elif_stmts), mv(else_stmts));
}

std::unique_ptr<Stmt> Parser::parse_new_scope(bool in_loop)
{
	tkns.match_token(Token::Specifier::BraceL);
	std::vector<uptr<Stmt>> stmts = parse_allowed_func_stmts(in_loop);
	tkns.match_token(Token::Specifier::BraceR);
	return std::make_unique<ScopeStmt>(mv(stmts));
}

std::unique_ptr<Stmt> Parser::parse_while_loop_stmt()
{
	tkns.match_token(Token::Specifier::KwWhile);
	tkns.match_token(Token::Specifier::RParenL);
	auto expr = expr_parser.parse();
	tkns.match_token(Token::Specifier::RParenR);
	tkns.match_token(Token::Specifier::BraceL);
	auto stmts = parse_allowed_func_stmts(true); 
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
	auto stmts = parse_allowed_func_stmts(true);
	tkns.match_token(Token::Specifier::BraceR); // }
	return std::make_unique<ForStmt>(mv(decl_stmt), mv(fst_expr), mv(snd_expr),mv(stmts));
}

// var_decl_def | expr_stmt | return_stmt
std::unique_ptr<Stmt> Parser::parse_allowed_func_stmt(bool in_loop)
{
	auto la1 = tkns.lookahead(1).type;
	auto la2 = tkns.lookahead(2).type;
	auto la3 = tkns.lookahead(3).type;
	switch (la1)
	{
	case Token::Specifier::BraceL:
		return parse_new_scope(in_loop);
	case Token::Specifier::KwContinue:
		return parse_continue_stmt();
	case Token::Specifier::KwMatch:
		return parse_match_stmt(in_loop);
	case Token::Specifier::KwReturn:
		return parse_return_stmt();
	case Token::Specifier::KwFor:
		return parse_for_loop_stmt();
	case Token::Specifier::KwWhile:
		return parse_while_loop_stmt();
	case Token::Specifier::KwIf:
		return parse_if_stmt(in_loop);
	case Token::Specifier::Ident:
	{
		auto parsed = try_parse<std::unique_ptr<Stmt>>([&]() {return parse_decl_operator_stmt(); });
		if (parsed)
			return parsed;
		//if (la2 == Token::Specifier::Asterix // e.g. int*
		//	|| la2 == Token::Specifier::Less // e.g. List<
		//	|| la2 == Token::Specifier::DeclCpy // e.g. a :=
		//	|| la2 == Token::Specifier::Colon // e.g. a :
		//	|| la2 == Token::Specifier::
		//	|| la3 == Token::Specifier::DeclCpy // int a :=
		//	|| la3 == Token::Specifier::Equal
		//	|| la3 == Token::Specifier::Colon)  // int c : 
		//	return parse_decl_operator_stmt(); // Also parses decl
		//if (la2 == Token::Specifier::BracketL) // e.g int[
		//{
		//	// can be something like int[4] = 4; or int[4][4];
		//	if (tkns.lookahead(5).type == Token::Specifier::Equal) // int <- 1, [ <-2, number <- 3, ] <- 4, = <- 5
		//		return parse_expr_stmt();
		//	else return parse_decl_stmt();
		//}
		//else if (la2 == Token::Specifier::Ident) // int a;
		//	return parse_decl_stmt();
	}
	break;
	case Token::Specifier::KwAuto:
	case Token::Specifier::KwFptr:
		return parse_decl_operator_stmt();
	default:
		break;
	}
	return parse_expr_stmt();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_allowed_func_stmts(bool in_loop)
{
	std::vector<std::unique_ptr<Stmt>> stmts;
	while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
	{
		stmts.push_back(parse_allowed_func_stmt(in_loop));
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
	if (tkns.lookahead(1).type == Token::Specifier::Semicolon)
	{
		tkns.eat();
		return std::make_unique<ReturnStmt>(nullptr, rk);
	}
	auto out = std::make_unique<ReturnStmt>(expr_parser.parse(),rk);
	tkns.match_token(Token::Specifier::Semicolon);
	return out;
}
