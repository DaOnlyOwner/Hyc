#pragma once
#include "Ast.h"
#include "Scopes.h"

class MemberCollector : public IAstVisitor
{
public:
	MemberCollector(Scopes& sc,NamespaceStmt& ns)
		:scopes(sc),ns(ns){}
private:
	virtual void visit(CollectionStmt& coll_def) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(FuncDefStmt& fdecl) override;
	
	Scopes& scopes;
	NamespaceStmt& ns;
	CollectionStmt* current_collection;
};

void collect_members(NamespaceStmt& ns, Scopes& sc);
