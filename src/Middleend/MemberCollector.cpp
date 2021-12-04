#include "MemberCollector.h"

void MemberCollector::visit(CollectionStmt& coll_def)
{
	current_collection = &coll_def;
	for (auto& p : coll_def.stmts) p->accept(*this);
}

void MemberCollector::visit(DeclStmt& decl)
{
	scopes.add(current_collection, &decl);
}

// Dont collect decls of function definitions.
void MemberCollector::visit(FuncDeclStmt& fdecl)
{
}
