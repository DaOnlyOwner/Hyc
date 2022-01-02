#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "Tree.h"

class DeclarationsCollectorTypes : public IAstVisitor
{
public:
	DeclarationsCollectorTypes(Scopes& sc, Tree<TypeDefStmt*>& type_hierachy)
		:scopes(sc),type_hierachy(type_hierachy) {}

private:
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(TypeDefStmt& coll_def) override;
	virtual void visit(FuncDefStmt& def) override;
	Scopes& scopes;
	Tree<TypeDefStmt*>& type_hierachy;
};

void collect_types(NamespaceStmt& ns,Scopes& sc, Tree<TypeDefStmt*>& type_hierachy);