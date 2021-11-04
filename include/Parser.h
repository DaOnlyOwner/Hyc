#pragma once
#include "Lexer.h"
#include "GenericPrattParser.h"
#include "Ast.h"


typedef GenericPrattParser<Expr> ExprParser;

// TODO: Correctness: Check if an integer / float token is of valid length
class Parser
{
public:
	Parser(Lexer& token_source, const std::string& filename);
	std::unique_ptr<Stmt> parse();

private:
	std::unique_ptr<Stmt> parse_compilation_unit();
	std::unique_ptr<Stmt> parse_inferred_def_stmt(); // Also parses definition
	std::unique_ptr<Stmt> parse_def_stmt();
	std::unique_ptr<Stmt> parse_function_def_stmt();
	std::unique_ptr<Stmt> parse_stmt();
	std::unique_ptr<Stmt> parse_expr_stmt();
	std::unique_ptr<Stmt> parse_return_stmt();

	Lexer& m_token_source; 
	ExprParser m_expr_parser;
	std::string file;
};



