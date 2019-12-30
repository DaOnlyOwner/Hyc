#pragma once
#include <memory>
#include "Token.h"
#include <vector>
#include <algorithm>

#define IMPL_VISITOR virtual void accept(IAstVisitor& visitor) override {visitor.visit(*this);}

template <typename TRet, typename TArg>
inline std::unique_ptr<TRet> ast_as(std::unique_ptr<TArg> arg)
{
	return static_cast<std::unique_ptr<TRet>>(std::move(arg));
}

template<typename T>
using uptr = std::unique_ptr<T>;

// Couldn't get std::move alias to work, so now here is a macro
#define mv(arg) std::move(arg)

struct IAstVisitor
{
	virtual void visit(struct FloatLiteralExpr& lit) = 0;
	virtual void visit(struct IntegerLiteralExpr& lit) = 0;
	virtual void visit(struct BinOpExpr& bin_op) = 0;
	virtual void visit(struct PrefixOpExpr& pre_op) = 0;
	virtual void visit(struct PostfixOpExpr& post_op) = 0;
	virtual void visit(struct InferredDeclStmt& decl_inferred) = 0;
	virtual void visit(struct IdentPattern& ident) = 0;
	virtual void visit(struct IdentExpr& ident) = 0;
	virtual void visit(struct NamespaceStmt& namespace_stmt) = 0;
	virtual void visit(struct FuncCallExpr& func_call_expr) = 0;
	virtual void visit(struct FuncDefStmt& func_call_def_stmt) = 0;
	virtual void visit(struct ReturnStmt& ret_stmt) = 0;
	virtual void visit(struct ExprStmt& expr_stmt) = 0;

};

struct Node
{
	virtual void accept(IAstVisitor& visitor) = 0;
	virtual ~Node() = default;
};

// Patterns

struct Pattern : Node {};

struct IdentPattern : Pattern
{
	IdentPattern(const Token& ident)
		:ident(ident){}
	Token ident;
	IMPL_VISITOR
};

// Expressions

struct Expr : Node{};

struct BinOpExpr : Expr
{
	BinOpExpr(const Token& op, uptr<Expr> lh, uptr<Expr> rh)
		:op(op),lh(mv(lh)),rh(mv(rh)){}
	Token op;
	uptr<Expr> lh, rh;
	IMPL_VISITOR
};

struct PrefixOpExpr : Expr
{
	PrefixOpExpr(const Token& op, uptr<Expr> lh)
		:op(op),lh(mv(lh)){}
	Token op;
	uptr<Expr> lh;
	IMPL_VISITOR
};

struct PostfixOpExpr : Expr
{
	PostfixOpExpr(const Token& op, uptr<Expr> rh)
		:op(op), rh(mv(rh)) {}
	Token op;
	uptr<Expr> rh;
	IMPL_VISITOR
};

struct IntegerLiteralExpr : Expr
{
	IntegerLiteralExpr(const Token& token) 
		: integer_literal(token){}
	Token integer_literal;
	IMPL_VISITOR
};

struct FloatLiteralExpr : Expr
{
	FloatLiteralExpr(const Token& token)
		: float_literal(token){}
	Token float_literal;
	IMPL_VISITOR
};

struct IdentExpr : Expr
{
	IdentExpr(const Token& token)
		: ident(token){}
	Token ident;
	IMPL_VISITOR
};

struct FuncCallExpr : Expr
{
	FuncCallExpr(const Token& name, std::vector<uptr<Expr>> arg_list)
		: name(name), arg_list(std::move(arg_list)){}
	Token name;
	std::vector<uptr<Expr>> arg_list;
	IMPL_VISITOR
};


// Statements

struct Stmt : Node {};

struct InferredDeclStmt : Stmt
{
	InferredDeclStmt(uptr<Pattern> bind_to, uptr<Expr> expr)
		:bind_to(mv(bind_to)), expr(mv(expr)) {}
	uptr<Pattern> bind_to;
	uptr<Expr> expr;
	IMPL_VISITOR
};

struct NamespaceStmt : Stmt
{
	NamespaceStmt(std::vector<uptr<Stmt>>&& stmts, const Token& name)
		:stmts(mv(stmts)),name(name) {}
	NamespaceStmt(const Token& name)
		:name(name){}
	std::vector<uptr<Stmt>> stmts;
	Token name;
	IMPL_VISITOR
};

struct FuncDefStmt : Stmt
{
	FuncDefStmt(const Token& type, const Token& name, std::vector<std::pair<Token, Token>>&& arg_list_type_ident, std::vector<uptr<Stmt>>&& body)
		: type(type), name(name), arg_list_type_ident(mv(arg_list_type_ident)), body(mv(body)){}
	Token type;
	Token name;
	std::vector<std::pair<Token, Token>> arg_list_type_ident;
	std::vector<uptr<Stmt>> body;
	IMPL_VISITOR
};

struct ExprStmt : Stmt
{
	ExprStmt(uptr<Expr>&& expr)
		: expr(mv(expr)){}
	uptr<Expr> expr;
	IMPL_VISITOR
};

struct ReturnStmt : Stmt
{
	ReturnStmt(uptr<Expr>&& returned_expr)
		: returned_expr(mv(returned_expr)){}
	uptr<Expr> returned_expr;
	IMPL_VISITOR
};











