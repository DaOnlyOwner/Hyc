#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"
#include <variant>

// TODO: Rename this
class ElideReturnCopy : public IAstVisitor, 
	public ValueStorageStack<std::unique_ptr<Expr>,std::unique_ptr<Expr>>
{
public:
	ElideReturnCopy(Scopes& scopes) :scopes(scopes), ValueStorageStack(this) {}
private:
	Scopes& scopes;
	std::vector<uptr<Stmt>>* stmts = nullptr;
	size_t ret_idx = 0;


	virtual void visit(BinOpExpr& bin) override;
	virtual void visit(PrefixOpExpr& pre) override;
	virtual void visit(FuncCallExpr& fce) override;
	virtual void visit(PostfixOpExpr& post) override;
	virtual void visit(ArraySubscriptExpr& expr) override;

	virtual void visit(ExprStmt& expr_stmt) override;
	virtual void visit(ReturnStmt& rt) override;
	virtual void visit(IfStmt& stmt) override;
	virtual void visit(WhileStmt& stmt) override;
	virtual void visit(ScopeStmt& stmt) override;
	virtual void visit(FuncDefStmt& stmt) override;

	void handle_descend(std::vector<uptr<Stmt>>& stmts);
};

void llvm_elide_return_copy(Scopes& sc, NamespaceStmt& ns);

