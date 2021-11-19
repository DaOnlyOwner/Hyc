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
	Lexer& get_lexer() { return tkns; }
	const Lexer& get_lexer() const { return tkns; }
	std::unique_ptr<Stmt> parse_compilation_unit();
	std::unique_ptr<Stmt> parse_function_def_stmt();
	std::unique_ptr<Stmt> parse_function_decl_stmt();
	std::unique_ptr<Stmt> parse_continue_stmt();
	std::unique_ptr<FuncDefStmt> parse_function_decl_stmt_part();
	std::unique_ptr<Stmt> parse_expr_stmt();
	std::unique_ptr<Stmt> parse_return_stmt();
	std::unique_ptr<Stmt> parse_decl_operator_stmt(); 
	std::unique_ptr<Stmt> parse_decl_stmt();
	std::unique_ptr<Stmt> parse_struct_def();
	std::unique_ptr<Stmt> parse_namespace_stmt();
	std::unique_ptr<Stmt> parse_contract_def();
	std::unique_ptr<Stmt> parse_contract_impl_block();
	std::unique_ptr<Stmt> parse_if_stmt();
	std::unique_ptr<Stmt> parse_while_loop_stmt();
	std::unique_ptr<Stmt> parse_for_loop_stmt();
	//std::unique_ptr<Stmt> parse_switch_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_namespace_stmts();
	std::unique_ptr<Stmt> parse_allowed_namespace_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_contract_stmts();
	std::vector<std::unique_ptr<Stmt>> parse_func_def_list();
	std::unique_ptr<Stmt> parse_allowed_loop_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_loop_stmts();
	std::unique_ptr<Stmt> parse_allowed_func_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_func_stmts();
	std::vector<GenericInfo>  parse_generic_list();
	std::unique_ptr<TypeSpec> parse_type_spec();
	GenericInfo parse_generic_param();
	std::unique_ptr<TypeSpec> Parser::parse_type_spec_part();
	std::vector<GenericInfo> parse_comma_separated_ident_list();
private:
	Lexer& tkns; 
	ExprParser expr_parser;
	std::string file;
};



