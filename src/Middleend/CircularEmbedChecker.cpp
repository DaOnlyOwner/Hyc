#include "CircularEmbedChecker.h"
#include "Messages.h"

void CircularEmbedChecker::visit(TypeDefStmt& td)
{
	embedded_in = &td;
	for (auto& p : td.stmts)
	{
		p->accept(*this);
	}
}

void CircularEmbedChecker::visit(DeclStmt& decl)
{
	if (decl.type.is_base_type() && !decl.type.is_predefined(scopes))
	{
		if (!type_hier.add(decl.type.get_base_type(), embedded_in))
		{
			Messages::inst().trigger_9_e1(decl.name, embedded_in->name.text);
		}
	}
}

void CircularEmbedChecker::visit(UnionDeclStmt& udecl)
{
	udecl.decl_stmt->accept(*this);
}

void check_circular_embed(NamespaceStmt& ns, Tree<TypeDefStmt*>& tree, Scopes& sc)
{
	CircularEmbedChecker cec(tree, sc);
	ns.accept(cec);
}
