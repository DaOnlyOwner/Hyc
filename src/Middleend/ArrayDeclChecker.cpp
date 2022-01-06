#include "ArrayDeclChecker.h"
#include "Messages.h"

void ArrayDeclChecker::visit(DeclStmt& decl)
{
	decl.type_spec->accept(*this);
	if (!was_not_array)
	{
		Messages::inst().trigger_10_e1(decl.type_spec->get_ident_token());
	}
}

void ArrayDeclChecker::visit(FuncDefStmt& func_def)
{
	if (!func_def.decl->generic_list.empty()) return;
	for (auto& p : func_def.decl->arg_list) p.decl->type_spec->accept(*this);
}

void ArrayDeclChecker::visit(ArrayTypeSpec& array_type_spec)
{
	was_not_array = false;
}

void ArrayDeclChecker::visit(BaseTypeSpec& bt)
{
	was_not_array = true;
}

void ArrayDeclChecker::visit(PointerTypeSpec& pts)
{
	was_not_array = true;
}

void ArrayDeclChecker::visit(FptrTypeSpec& fptr)
{
	was_not_array = true;
}

void check_array_decl(NamespaceStmt& ns, Scopes& scopes)
{
	ArrayDeclChecker adc(scopes, ns);
	ns.accept(adc);
}
