#include "MemberCollector.h"
#include "Messages.h"
#include "TypeCreator.h"

void MemberCollector::visit(TypeDefStmt& coll_def)
{
	if (!coll_def.generic_params.empty()) return; // Don't collect members of generic collections
	current_collection = &coll_def;
	idx = 0;
	for (auto& p : coll_def.stmts) p->accept(*this);
}

// TODO: Handle that you cannot declare types of void.
void MemberCollector::visit(DeclStmt& decl)
{
	auto [t, succ] = create_type(*decl.type_spec, scopes, ns, false);
	assert(succ);
	decl.type = t;

	bool succAdd = false;
	succAdd = scopes.add(current_collection, &decl, idx);
	if (!succAdd)
	{
		Messages::inst().trigger_7_e1(decl.name, current_collection->name.text);
	}
	idx++;
}

void MemberCollector::visit(UnionDeclStmt& udecl)
{
	if (udecl.decl_stmt->type_spec == nullptr)
	{
		udecl.decl_stmt->type = scopes.get_type("void");
	}
	else
	{
		auto [t, succ] = create_type(*udecl.decl_stmt->type_spec, scopes, ns, false);
		assert(succ);
		udecl.decl_stmt->type = t;
	}
	bool succAdd = false;
	succAdd = scopes.add(current_collection, &udecl);
	if (!succAdd)
	{
		Messages::inst().trigger_7_e1(udecl.decl_stmt->name, current_collection->name.text);
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
