#include "MemberCollector.h"
#include "Messages.h"
#include "TypeCreator.h"

void MemberCollector::visit(CollectionStmt& coll_def)
{
	if (!coll_def.generic_params.empty()) return; // Don't collect members of generic collections
	current_collection = &coll_def;
	for (auto& p : coll_def.stmts) p->accept(*this);
}

void MemberCollector::visit(DeclStmt& decl)
{
	auto[t,succ]=create_type(*decl.type_spec, scopes, ns, false);
	assert(succ);
	decl.type = t;
	bool succAdd = scopes.add(current_collection, &decl);
	if (!succAdd)
	{
		Messages::inst().trigger_7_e1(decl.name, current_collection->name.text);
	}
}

// Dont collect decls of function definitions.
void MemberCollector::visit(FuncDefStmt& fdecl)
{
}

void MemberCollector::visit(NamespaceStmt& ns)
{
	for (int i = entry; i < ns.stmts.size(); i++)
	{
		ns.stmts[i]->accept(*this);
	}
}


void collect_members(NamespaceStmt& ns, Scopes& sc)
{
	MemberCollector mc(sc,ns);
	ns.accept(mc);
}

void collect_members(NamespaceStmt& ns, Scopes& sc, size_t n)
{
	MemberCollector mc(sc, ns,n);
	ns.accept(mc);
}
