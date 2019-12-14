#pragma once
#include "Lexer.h"
#include "GenericPrattParser.h"
#include "Ast.h"


typedef GenericPrattParser<Expr> ExprParser;
typedef GenericPrattParser<Pattern> PatternParser;

class Parser
{
public:
	Parser(Lexer& token_source);
	std::unique_ptr<Stmt> parse();

private:
	std::unique_ptr<Stmt> parse_compilation_unit();
	std::unique_ptr<Stmt> parse_inferred_decl_stmt();

	Lexer& m_token_source; 
	ExprParser m_expr_parser;
	PatternParser m_pattern_parser;	
};



