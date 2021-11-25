#pragma once
#include "Lexer.h"
#include "GenericPrattParser.h"
#include "Ast.h"
#include "ValuePtr.h"

typedef GenericPrattParser<Expr> ExprParser;

// TODO: Correctness: Check if an integer / float token is of valid length
class Parser
{
public:
	Parser(Lexer& token_source, const std::string& filename);
	ValuePtr<Stmt> parse();
	Lexer& get_lexer() { return tkns; }
	const Lexer& get_lexer() const { return tkns; }
	ValuePtr<Stmt> parse_compilation_unit();
	ValuePtr<Stmt> parse_function_def_stmt();
	ValuePtr<Stmt> parse_function_decl_stmt();
	ValuePtr<Stmt> parse_continue_stmt();
	ValuePtr<FuncDeclStmt> parse_function_decl_stmt_part();
	ValuePtr<Stmt> parse_expr_stmt();
	ValuePtr<Stmt> parse_return_stmt();
	ValuePtr<Stmt> parse_decl_operator_stmt(); 
	ValuePtr<Stmt> parse_decl_stmt();
	ValuePtr<Stmt> parse_struct_def();
	ValuePtr<Stmt> parse_union_def();
	
	// e.g. struct {int a;}
	template<typename TRet, typename Fn>
	ValuePtr<TRet> parse_attr_collection(Token::Specifier spec, Fn fn)
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
		return make_value<TRet>(mv(name), mv(generic_parameters), mv(decls_inside));
	}

	ValuePtr<Stmt> parse_match_stmt(bool in_loop);
	MatchCase parse_match_case(bool in_loop);
	ValuePtr<Stmt> parse_namespace_stmt();
	ValuePtr<Stmt> parse_if_stmt();
	ValuePtr<Stmt> parse_while_loop_stmt();
	ValuePtr<Stmt> parse_for_loop_stmt();
	//ValuePtr<Stmt> parse_switch_stmt();
	std::vector<ValuePtr<Stmt>> parse_function_body();
	std::vector<ValuePtr<Stmt>> parse_allowed_namespace_stmts();
	ValuePtr<Stmt> parse_allowed_top_level_stmt();
	ValuePtr<Stmt> parse_allowed_namespace_stmt();
	std::vector<ValuePtr<Stmt>> parse_func_def_list();
	ValuePtr<Stmt> parse_allowed_loop_stmt();
	std::vector<ValuePtr<Stmt>> parse_allowed_loop_stmts();
	ValuePtr<Stmt> parse_allowed_func_stmt();
	std::vector<ValuePtr<Stmt>> parse_allowed_func_stmts();
	ValuePtr<TypeSpec> parse_type_spec();
	ValuePtr<TypeSpec> parse_type_spec_part();
	std::vector<GenericInfo> parse_comma_separated_ident_list();
private:
	Lexer& tkns; 
	ExprParser expr_parser;
	std::string file;
};



