#include "Parser.h"
#include "GenericPrattParser.h"
#include "Precedence.h"
#include "DebugPrint.h"
#include <cstdlib>

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

	std::pair<bool, Primitive::Specifier> eval_integer_val(const Token& token)
	{
		const std::string& integer_text = token.text;
		errno = 0;
		if (token.text[0] == '-')
		{
			auto val = strtoll(token.text.c_str(), nullptr, 0);
			if (errno != 0)
			{
				return { false,Primitive::Specifier::Count };
			}
			return { true,Primitive::Specifier::int_ };
		}

		else
		{
			errno = 0;
			auto val= strtoull(token.text.c_str(), nullptr, 0);
			if (errno != 0)
			{
				return { false,Primitive::Specifier::Count };
			}
			return { true,Primitive::Specifier::uint };
		}
	}

	PrefixOperation<Expr> integer_lit{ (int)ExprPrecedence::integer_lit, [](PrefixExprFnArgs)
	{
			auto [succ,spec] = eval_integer_val(token);
			if (!succ)
			{
				auto descr = Error::FromToken(token);
				descr.Message = fmt::format("{} doesn't fit into an i64 or u64", token.text);
				descr.Hint = "You need to reduce the size of the literal, so that it fits into either an i64 or an u64";
				Error::SyntacticalError(descr);
			}
		return ast_as<Expr>(std::make_unique<IntegerLiteralExpr>(token,spec));
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
}

Parser::Parser(Lexer& token_source, const std::string& filename)
	:m_expr_parser(token_source),
	m_token_source(token_source)
{
	m_expr_parser.add_operation(Token::Specifier::Asterix, productGroup);
	m_expr_parser.add_operation(Token::Specifier::Plus, sumGroup);
	m_expr_parser.add_operation(Token::Specifier::Float, float_lit);
	m_expr_parser.add_operation(Token::Specifier::Integer, integer_lit);
	m_expr_parser.add_operation(Token::Specifier::Minus, unary_operator);
	m_expr_parser.add_operation(Token::Specifier::Plus, unary_operator);
	m_expr_parser.add_operation(Token::Specifier::Ident, ident_expr);

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
	while (m_token_source.lookahead(1).type != Token::Specifier::Eof)
	{
		nms->stmts.push_back(parse_stmt());
	}
	return mv(nms);
}

// ident := expr ;
std::unique_ptr<Stmt> Parser::parse_inferred_def_stmt()
{
	auto lh = m_token_source.match_token(Token::Specifier::Ident);
	m_token_source.match_token(Token::Specifier::Decl);
	auto rh = m_expr_parser.parse();
	m_token_source.match_token(Token::Specifier::Semicolon);
	return std::make_unique<InferredDefStmt>(mv(lh),mv(rh));
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
		
		param_list.push_back(std::make_pair(arg_type, arg_name));

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

// type ident '=' expr
std::unique_ptr<Stmt> Parser::parse_def_stmt()
{
	auto type = m_token_source.match_token(Token::Specifier::Ident); // Type
	auto name = m_token_source.match_token(Token::Specifier::Ident); // Ident
	m_token_source.match_token(Token::Specifier::Equal); // =
	auto rh = m_expr_parser.parse(); // expr
	m_token_source.match_token(Token::Specifier::Semicolon);
	return std::make_unique<DefStmt>(type, name, std::move(rh));
}

std::unique_ptr<Stmt> Parser::parse_stmt()
{
	if (m_token_source.lookahead(1).type == Token::Specifier::kw_return)
		return parse_return_stmt();
	else if (m_token_source.lookahead(2).type == Token::Specifier::Decl)
		return parse_inferred_def_stmt();
	else if (m_token_source.lookahead(1).type == Token::Specifier::Ident && m_token_source.lookahead(2).type == Token::Specifier::Ident && m_token_source.lookahead(3).type == Token::Specifier::Equal)
		return parse_def_stmt();
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
	auto rk = m_token_source.match_token(Token::Specifier::kw_return);
	auto out = std::make_unique<ReturnStmt>(m_expr_parser.parse(),rk);
	m_token_source.match_token(Token::Specifier::Semicolon);
	return out;
}
