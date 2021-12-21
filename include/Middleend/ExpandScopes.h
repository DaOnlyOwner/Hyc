#pragma once
#include "Ast.h"
#include "Scopes.h"

class ExpandScopes : public IAstVisitor
{
public:
	ExpandScopes(Scopes& sc, size_t entry)
		:scopes(sc),entry(entry) {}
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(FuncDefStmt& func_def) override;
	virtual void visit(TypeDefStmt& struct_def_stmt) override;
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(ForStmt& for_stmt) override;
	virtual void visit(MatchStmt& match) override;
	virtual void visit(ScopeStmt& sc) override;
	Scopes& scopes;
	size_t entry = 0;
};

void expand_scopes(NamespaceStmt& ns, Scopes& sc,size_t entry = 0);