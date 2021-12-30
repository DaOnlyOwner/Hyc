#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"

class PromoteToSret : public IAstVisitor, public ValueStorage<std::pair<uptr<Stmt>,uptr<Stmt>>>
{
public:
	PromoteToSret(Scopes& scopes)
		:scopes(scopes),ValueStorage(this) {}

private:
	Scopes& scopes;
	std::unique_ptr<PrefixOpExpr> expr;

	virtual void visit(FuncDefStmt& func_def) override;
	virtual void visit(TypeDefStmt& td) override {}
	virtual void visit(ReturnStmt& rs) override;
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(ScopeStmt& scope) override;
	void make_new_stmt(std::vector<uptr<Stmt>>& stmts);
};

void llvm_promote_to_sret(Scopes& sc, NamespaceStmt& ns);

