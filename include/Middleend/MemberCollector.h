#pragma once
#include "Ast.h"
#include "Scopes.h"

class MemberCollector : public IAstVisitor
{
public:
	MemberCollector(Scopes& sc,NamespaceStmt& ns)
		:scopes(sc),ns(ns){}
	MemberCollector(Scopes& sc, NamespaceStmt& ns, size_t entry)
		:scopes(sc),ns(ns),entry(entry) {}
private:
	virtual void visit(TypeDefStmt& coll_def) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(UnionDeclStmt& udecl) override;
	virtual void visit(FuncDefStmt& fdecl) override;
	virtual void visit(NamespaceStmt& ns) override;
	
	Scopes& scopes;
	NamespaceStmt& ns;
	TypeDefStmt* current_collection = nullptr;
	size_t entry = 0;
	size_t idx = 0;
};

void collect_members(NamespaceStmt& ns, Scopes& sc);
void collect_members(NamespaceStmt& ns, Scopes& sc, size_t n);