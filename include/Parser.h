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
	std::unique_ptr<FuncDeclStmt> parse_function_decl_stmt_part();
	std::unique_ptr<Stmt> parse_expr_stmt();
	std::unique_ptr<Stmt> parse_return_stmt();
	std::unique_ptr<Stmt> parse_decl_operator_stmt(); 
	std::unique_ptr<Stmt> parse_decl_stmt();
	std::unique_ptr<Stmt> parse_struct_def();
	std::unique_ptr<Stmt> parse_union_def();
	
	template<typename TRet, typename Fn>
	std::unique_ptr<TRet> parse_attr_collection(Token::Specifier spec, Fn fn)
	{
		tkns.match_token(spec);
		auto& name = tkns.match_token(Token::Specifier::Ident);
		std::vector<GenericInfo> generic_parameters;
		if (tkns.lookahead(1).type == Token::Specifier::Less)
		{
			tkns.eat();
			generic_parameters = parse_comma_separated_ident_list();
			tkns.match_token(Token::Specifier::Greater);
		}
		tkns.match_token(Token::Specifier::BraceL);
		std::vector<uptr<Stmt>> decls_inside;
		while (tkns.lookahead(1).type != Token::Specifier::BraceR && tkns.lookahead(1).type != Token::Specifier::Eof)
		{
			decls_inside.push_back(fn());
		}
		tkns.match_token(Token::Specifier::BraceR);
		return std::make_unique<TRet>(mv(name), mv(generic_parameters), mv(decls_inside));
	}


	std::unique_ptr<Stmt> parse_namespace_stmt();
	std::unique_ptr<Stmt> parse_if_stmt();
	std::unique_ptr<Stmt> parse_while_loop_stmt();
	std::unique_ptr<Stmt> parse_for_loop_stmt();
	//std::unique_ptr<Stmt> parse_switch_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_function_body();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_namespace_stmts();
	std::unique_ptr<Stmt> parse_allowed_namespace_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_func_def_list();
	std::unique_ptr<Stmt> parse_allowed_loop_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_loop_stmts();
	std::unique_ptr<Stmt> parse_allowed_func_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_func_stmts();
	std::unique_ptr<TypeSpec> parse_type_spec();
	GenericInfo parse_generic_param();
	std::unique_ptr<TypeSpec> Parser::parse_type_spec_part();
	std::vector<GenericInfo> parse_comma_separated_ident_list();
private:
	Lexer& tkns; 
	ExprParser expr_parser;
	std::string file;
};



