#pragma once
#include "Ast.h"
#include "ValueStorage.h"

// Transforms something like this a.b.c() into c(&(a.b))
// Transforms for loops into while loops <-- Probably not worth it.
class DesugarAccess : public IAstVisitor
{
public:
	DesugarAccess() = default;
private:
	virtual void visit(BinOpExpr& bin)override;
	virtual void visit(PrefixOpExpr& pre) override;
	virtual void visit(PostfixOpExpr& post) override;
	virtual void visit(FuncCallExpr& call) override;
};

class DesugarForStmt : public IAstVisitor, ValueStorage<uptr<ScopeStmt>>
{
public:
	DesugarForStmt()
		:ValueStorage<uptr<ScopeStmt>>(this) {}
private:
	virtual void visit(NamespaceStmt& ns);
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(MatchStmt& match) override;
	virtual void visit(ScopeStmt& sc) override;
	virtual void visit(FuncDefStmt& fds) override;
	virtual void visit(ForStmt& for_stmt) override;

	void update(std::vector<uptr<Stmt>>&);

};

class DesugarDecl : public IAstVisitor, ValueStorage<std::pair<uptr<DeclStmt>,uptr<ExprStmt>>>
{
public:
	DesugarDecl()
		:ValueStorage(this) {}
private:
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(MatchStmt& match) override;
	virtual void visit(ScopeStmt& sc) override;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
	virtual void visit(DeclCpyStmt& cpy) override;
	virtual void visit(DeclMvStmt& mv) override;
	virtual void visit(DeclInitStmt& init) override;

	void update(std::vector<uptr<Stmt>>& to_update);
	template<typename T>
	void make_new_stmts(T& decl,Token::Specifier op)
	{
		Token tk(decl.name);
		tk.type = op;
		tk.text = Token::Translate(op);
		uptr<DeclStmt> decl_new = std::make_unique<DeclStmt>(mv(decl.type), std::move(decl.name));
		uptr<BinOpExpr> bin_op = std::make_unique<BinOpExpr>(std::move(tk),
			std::make_unique<IdentExpr>(Token(decl_new->name)), mv(decl.expr));
		uptr<ExprStmt> sexpr = std::make_unique<ExprStmt>(mv(bin_op));
		RETURN(std::make_pair(mv(decl_new), mv(sexpr)));
	}
};

class DesugarCompound : public IAstVisitor
{
private:
	virtual void visit(CollectionStmt& coll_stmt) override;
	virtual void visit(BinOpExpr& expr) override;
	virtual void visit(PrefixOpExpr& expr) override;
	virtual void visit(PostfixOpExpr& expr) override;
	virtual void visit(FuncCallExpr& expr) override;
	virtual void visit(ArraySubscriptExpr& expr) override;
};

void desugar(NamespaceStmt& ns);