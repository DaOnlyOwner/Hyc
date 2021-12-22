#include "TaggedValueChecker.h"

void TaggedValueChecker::visit(TypeDefStmt& td)
{
	if (td.get_ct() == CollectionType::Struct
		|| !td.generic_params.empty()) return;

	if (!td.stmts.empty())
	{
		auto udecl = dynamic_cast<UnionDeclStmt*>(td.stmts.front().get());
		if (udecl->tagged_value.has_value())
			tagged_val = udecl->tagged_value.value().val;
		else tagged_val = 0;
	}

	for (auto& p : td.stmts)
	{
		p->accept(*this);
		tagged_val++;
	}
}

void TaggedValueChecker::visit(UnionDeclStmt& udecl)
{
	if (!udecl.tagged_value.has_value())
	{
		EvalIntegerResult eir;
		eir.amount_bits = 64;
		eir.is_signed = false;
		eir.val = tagged_val;
		udecl.tagged_value = eir;
	}
}

void TaggedValueChecker::visit(NamespaceStmt& ns)
{
	for (int i = entry; i < ns.stmts.size(); i++)
	{
		ns.stmts[i]->accept(*this);
	}
}

void check_tagged_values(NamespaceStmt& ns, size_t entry)
{
	TaggedValueChecker tvc(entry);
	ns.accept(tvc);
}
