#include "MemberCollector.h"
#include "Messages.h"

void MemberCollector::visit(CollectionStmt& coll_def)
{
	current_collection = &coll_def;
	for (auto& p : coll_def.stmts) p->accept(*this);
}

void MemberCollector::visit(DeclStmt& decl)
{
	bool succ = scopes.add(current_collection, &decl);
	if (!succ)
	{
		Messages::inst().trigger_7_e1(decl.name, current_collection->name.text);
	}
}

// Dont collect decls of function definitions.
void MemberCollector::visit(FuncDeclStmt& fdecl)
{
}

void collect_members(NamespaceStmt& ns, Scopes& sc)
{
	MemberCollector mc(sc);
	ns.accept(mc);
}