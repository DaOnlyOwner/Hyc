#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"
#include "DebugPrint.h"

// Expressions

// Infix Operators
#define InfixExprFnArgs ExprParser& parser, const Token& token, std::unique_ptr<Expr> lh
#define PrefixExprFnArgs ExprParser& parser, const Token& token

					 // Precedence  Right assoc?  Parsing function
InfixOperation<Expr> productGroup{(int)ExprPrecedence::productGroup, false, [](InfixExprFnArgs) {
	return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal((int)ExprPrecedence::productGroup));
} };

InfixOperation<Expr> sumGroup{(int)ExprPrecedence::sumGroup, false, [](InfixExprFnArgs) {
	return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal((int)ExprPrecedence::sumGroup));
} };


// "Prefix" Operators

PrefixOperation<Expr> float_lit{ (int)ExprPrecedence::float_lit, [](PrefixExprFnArgs)
{
	return ast_as<Expr>(std::make_unique<FloatLiteralExpr>(token));
} };

PrefixOperation<Expr> integer_lit{ (int)ExprPrecedence::integer_lit, [](PrefixExprFnArgs)
{
	return ast_as<Expr>(std::make_unique<IntegerLiteralExpr>(token));
} };

PrefixOperation<Expr> ident_expr{ (int)ExprPrecedence::ident_expr, [](PrefixExprFnArgs) {
	// Function call
	if (parser.get_token_source().lookahead(1).type == Token::Specifier::rparen_l)
	{
		parser.get_token_source().eat(); // '('
		std::vector<uptr<Expr>> arg_list;
		// Parse an argument list:
		while (parser.get_token_source().lookahead(1).type != Token::Specifier::rparen_r)
		{
			arg_list.push_back(parser.parse());
			if (parser.get_token_source().lookahead(1).type == Token::Specifier::comma)
				parser.get_token_source().eat();
			
		}

		parser.get_token_source().eat(); // ')'

		return static_cast<uptr<Expr>>(std::make_unique<FuncCallExpr>(token, std::move(arg_list)));
	}
	// Just an identifier.
    return static_cast<uptr<Expr>>(std::make_unique<IdentExpr>(token));
} };

PrefixOperation<Expr> unary_operator{ (int)ExprPrecedence::unary_op, [](PrefixExprFnArgs)
{
    return ast_as<Expr>(std::make_unique<PrefixOpExpr>(token, parser.parse_internal((int)ExprPrecedence::unary_op)));
} };

PrefixOperation<Expr> function_call{  };

// Patterns
#define InfixPatternFnArgs PatternParser& parser, const Token& token, std::unique_ptr<Pattern> lh
#define PrefixPatternFnArgs PatternParser& parser, const Token& token


PrefixOperation<Pattern> ident_pattern{ 10, [](PrefixPatternFnArgs) {
	return ast_as<Pattern>(std::make_unique<IdentPattern>(token));
} };


Parser::Parser(Lexer& token_source)
	:m_expr_parser(token_source),
	m_token_source(token_source),
	m_pattern_parser(token_source)
{
	m_expr_parser.add_operation(Token::Specifier::Asterix, productGroup);
	m_expr_parser.add_operation(Token::Specifier::Plus, sumGroup);
	m_expr_parser.add_operation(Token::Specifier::Float, float_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerU8, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerU16, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerU32, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerU64, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerS8, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerS16, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerS32, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::IntegerS64, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::Minus, unary_operator);
	m_expr_parser.add_operation(Token::Specifier::Plus, unary_operator);
	m_expr_parser.add_operation(Token::Specifier::Ident, ident_expr);
	m_pattern_parser.add_operation(Token::Specifier::Ident, ident_pattern);
}

std::unique_ptr<Stmt> Parser::parse()
{
	return parse_compilation_unit();
}

// stmt*
std::unique_ptr<Stmt> Parser::parse_compilation_unit()
{
	auto nms = std::make_unique<NamespaceStmt>(Token(Token::Specifier::Ident, "GLOBAL", "", 0, 0, 0, 0));
	while (m_token_source.lookahead(1).type != Token::Specifier::Eof)
	{
		nms->stmts.push_back(parse_stmt());
	}
	return mv(nms);
}

// pattern := expr ;
std::unique_ptr<Stmt> Parser::parse_inferred_decl_stmt()
{
	auto lh = m_pattern_parser.parse();
	m_token_source.match_token(Token::Specifier::Decl);
	auto rh = m_expr_parser.parse();
	m_token_source.match_token(Token::Specifier::Semicolon);
	return std::make_unique<InferredDeclStmt>(mv(lh),mv(rh));
}

// type ident (type ident1, type ident2,...,){stmts}
std::unique_ptr<Stmt> Parser::parse_function_def_stmt()
{
	auto type = m_token_source.match_token(Token::Specifier::Ident); // Type
	auto name = m_token_source.match_token(Token::Specifier::Ident); // name
	
	std::vector<std::pair<Token, Token>> param_list;

	m_token_source.match_token(Token::Specifier::rparen_l);
		
	while (m_token_source.lookahead(1).type != Token::Specifier::rparen_r)
	{
		auto arg_type = m_token_source.match_token(Token::Specifier::Ident); // Type
		auto arg_name = m_token_source.match_token(Token::Specifier::Ident); // name
		
		param_list.push_back(std::make_pair(arg_name, arg_type));

		if (m_token_source.lookahead(1).type == Token::Specifier::comma) m_token_source.eat();
	}

	m_token_source.eat(); // ')'

	std::vector<uptr<Stmt>> body;
	
	m_token_source.match_token(Token::Specifier::brace_l); // '{'
	while (m_token_source.lookahead(1).type != Token::Specifier::brace_r)
	{
		body.push_back(parse_stmt());
	}

	m_token_source.eat(); // '}'
	return std::make_unique<FuncDefStmt>(type,name,std::move(param_list), std::move(body));
}

std::unique_ptr<Stmt> Parser::parse_stmt()
{
	if (m_token_source.lookahead(1).type == Token::Specifier::kw_return)
		return parse_return_stmt();
	else if (m_token_source.lookahead(2).type == Token::Specifier::Decl)
		return parse_inferred_decl_stmt();
	else if (m_token_source.lookahead(3).type == Token::Specifier::rparen_l)
		return parse_function_def_stmt();
	else return parse_expr_stmt();
}

std::unique_ptr<Stmt> Parser::parse_expr_stmt()
{
	auto out = std::make_unique<ExprStmt>(m_expr_parser.parse());
	m_token_source.match_token(Token::Specifier::Semicolon);
	return out;
}

std::unique_ptr<Stmt> Parser::parse_return_stmt()
{
	m_token_source.match_token(Token::Specifier::kw_return);
	auto out = std::make_unique<ReturnStmt>(m_expr_parser.parse());
	m_token_source.match_token(Token::Specifier::Semicolon);
	return out;
}
