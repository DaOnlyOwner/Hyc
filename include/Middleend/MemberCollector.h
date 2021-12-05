#pragma once
#include "Ast.h"
#include "Scopes.h"

class MemberCollector : public IAstVisitor
{
public:
	MemberCollector(Scopes& sc)
		:scopes(sc){}
private:
	virtual void visit(CollectionStmt& coll_def) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(FuncDeclStmt& fdecl) override;
	
	Scopes& scopes;
	CollectionStmt* current_collection;
};

void collect_members(NamespaceStmt& ns, Scopes& sc);
