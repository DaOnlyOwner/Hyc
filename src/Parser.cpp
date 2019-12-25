#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"

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
	if(parser.lookahead(1) == '(') 
    return std::make_unique<IdentExpr>(token);
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
