#pragma once
#include "Ast.h"
#include "Scopes.h"

class DeclarationsCollectorTypes : public IAstVisitor
{
public:
	DeclarationsCollectorTypes(Scopes& sc)
		:scopes(sc){}

private:
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct CollectionStmt& coll_def) override;
	Scopes& scopes;
};

void collect_types(NamespaceStmt& ns,Scopes& sc);