#pragma once
#include "Ast.h"
#include "Scopes.h"

class DeclarationsCollectorTypes : public IAstVisitor
{
public:
	DeclarationsCollectorTypes(Scopes& sc)
		:scopes(sc){}

private:
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(CollectionStmt& coll_def) override;
	virtual void visit(FuncDefStmt& def) override;
	Scopes& scopes;
};

void collect_types(NamespaceStmt& ns,Scopes& sc);