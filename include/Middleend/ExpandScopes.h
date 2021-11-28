#pragma once
#include "Ast.h"
#include "Scopes.h"

class ExpandScopes : public IAstVisitor
{
public:
	ExpandScopes(Scopes& sc)
		:scopes(sc){}
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct FuncDefStmt& func_def) override;
	virtual void visit(struct CollectionStmt& struct_def_stmt) override;
	virtual void visit(struct IfStmt& if_stmt) override;
	virtual void visit(struct WhileStmt& while_stmt) override;
	virtual void visit(struct ForStmt& for_stmt) override;
	virtual void visit(struct MatchStmt& match) override;
	Scopes& scopes;
};

void expand_scopes(NamespaceStmt& ns, Scopes& sc);