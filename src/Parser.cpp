#include "Parser.h"
#include "GenericPrattParser.h"


// Expressions

// Infix Operators
#define InfixExprFnArgs ExprParser& parser, const Token& token, std::unique_ptr<Expr> lh
#define PrefixExprFnArgs ExprParser& parser, const Token& token

					 // Precedence  Right assoc?  Parsing function
InfixOperation<Expr> product{ 4,      false,	 [](InfixExprFnArgs) {
	return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal(4));
} };

InfixOperation<Expr> sum{ 3,	  false,	 [](InfixExprFnArgs) {
	return std::make_unique<BinOpExpr>(token,mv(lh),parser.parse_internal(3));
} };


// "Prefix" Operators

PrefixOperation<Expr> float_lit{ 10, [](PrefixExprFnArgs)
{
	return ast_as<Expr>(std::make_unique<FloatLiteralExpr>(token));
} };

PrefixOperation<Expr> integer_lit{ 10, [](PrefixExprFnArgs)
{
	return ast_as<Expr>(std::make_unique<IntegerLiteralExpr>(token));
} };

PrefixOperation<Expr> ident_expr{ 10, [](PrefixExprFnArgs) {
	return std::make_unique<IdentExpr>(token);
} };

PrefixOperation<Expr> unary_operator{ 6, [](PrefixExprFnArgs)
{
	switch (token.type)
	{
	case Token::Specifier::Minus:
		return ast_as<Expr>(std::make_unique<PrefixOpExpr>(token, parser.parse_internal(6)));
	case Token::Specifier::Plus:
		return ast_as<Expr>(std::make_unique<PrefixOpExpr>(token, parser.parse_internal(6)));
	default:
		abort();
	}
} };


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
	m_expr_parser.add_operation(Token::Specifier::Asterix, product);
	m_expr_parser.add_operation(Token::Specifier::Plus, sum);
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
	const Token* token = &m_token_source.lookahead(1);
	auto nms = std::make_unique<NamespaceStmt>(Token(Token::Specifier::Ident, "GLOBAL", "", 0, 0, 0, 0));
	while (token->type != Token::Specifier::Eof)
	{
		nms->stmts.push_back(parse_inferred_decl_stmt());
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
