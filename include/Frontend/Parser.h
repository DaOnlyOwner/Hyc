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
	std::unique_ptr<NamespaceStmt> parse();
	Lexer& get_lexer() { return tkns; }
	const Lexer& get_lexer() const { return tkns; }

	std::unique_ptr<NamespaceStmt> parse_compilation_unit();
	std::unique_ptr<Stmt> parse_operator_def_stmt();
	std::unique_ptr<Stmt> parse_function_def_stmt();
	std::unique_ptr<Stmt> parse_function_decl_stmt();
	std::unique_ptr<Stmt> parse_continue_stmt();
	std::unique_ptr<FuncDeclStmt> parse_function_decl_stmt_part();
	std::unique_ptr<Stmt> parse_expr_stmt();
	std::unique_ptr<Stmt> parse_return_stmt();
	std::unique_ptr<Stmt> parse_decl_operator_stmt(); 
	std::unique_ptr<Stmt> parse_decl_stmt();
	std::unique_ptr<Expr> parse_init_list();
	std::unique_ptr<InitListStructExpr> parse_init_list_struct(uptr<TypeSpec> type_to_init);
	std::unique_ptr<InitListArrayExpr> parse_init_list_array(uptr<TypeSpec> type_to_init);
	std::unique_ptr<Stmt> parse_union_decl_stmt();
	std::unique_ptr<Stmt> parse_struct_def();
	std::unique_ptr<Stmt> parse_union_def();
	
	// e.g. struct {int a;}
	template<typename Fn>
	std::unique_ptr<Stmt> parse_attr_collection(Token::Specifier spec, Fn fn)
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
		return std::make_unique<TypeDefStmt>(mv(name), mv(generic_parameters), mv(decls_inside),spec == Token::Specifier::KwStruct ? CollectionType::Struct : CollectionType::Union);
	}

	std::unique_ptr<Stmt> parse_match_stmt(bool in_loop);
	MatchCase parse_match_case(bool in_loop);
	std::unique_ptr<Stmt> parse_namespace_stmt();
	std::unique_ptr<Stmt> parse_if_stmt(bool in_loop);
	std::unique_ptr<Stmt> parse_new_scope(bool in_loop);
	std::unique_ptr<Stmt> parse_while_loop_stmt();
	std::unique_ptr<Stmt> parse_for_loop_stmt();
	//std::unique_ptr<Stmt> parse_switch_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_function_body();
	std::vector<std::unique_ptr<Stmt>> parse_allowed_namespace_stmts();
	std::unique_ptr<Stmt> parse_allowed_top_level_stmt();
	std::unique_ptr<Stmt> parse_allowed_namespace_stmt();
	std::vector<std::unique_ptr<Stmt>> parse_func_def_list();
	std::unique_ptr<Stmt> parse_allowed_func_stmt(bool in_loop=false);
	std::vector<std::unique_ptr<Stmt>> parse_allowed_func_stmts(bool in_loop);
	std::unique_ptr<TypeSpec> parse_type_spec();
	std::unique_ptr<BaseTypeSpec> parse_base_type_spec();
	std::unique_ptr<TypeSpec> parse_type_spec_part();
	// returns true if the returned expr is an ident, else false if the returned expr is a fptr ident
	std::pair<bool,std::unique_ptr<Expr>> parse_ident_expr(const Token& token);
	std::vector<GenericInfo> parse_comma_separated_ident_list();

	void save();

	void backtrack();

	template<typename TReturn, typename Fn>
	TReturn try_parse(Fn fn)
	{
		save();
		auto emit_before = Error::DoEmit;
		Error::SetEmit(false);
		try
		{
			auto out = fn();
			savepoints.pop_back();
			Error::SetEmit(emit_before);
			return out;
		}
		catch (Error::SyntaxErrorException& e)
		{
			backtrack();
			Error::SetEmit(emit_before);
			return nullptr;
		}
	}

private:
	Lexer& tkns; 
	ExprParser expr_parser;
	std::string file;
	std::vector<Lexer::TokenPos> savepoints;
};



