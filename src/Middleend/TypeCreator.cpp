#include "TypeCreator.h"
#include "DebugPrint.h"
#include "GenericInstantiation.h"
#include <cassert>
#include "ValuePtr.h"
#include "Messages.h"

std::pair<Type, bool> create_type(TypeSpec& ts, Scopes& scopes, NamespaceStmt& ns, bool instantiate_generic)
{
	TypeCreator tc(ts, scopes, ns, instantiate_generic);
	auto type = tc.get(ts);
	type.reverse();
	return { std::move(type),tc.get_succ()};
}

void TypeCreator::visit(PointerTypeSpec& pt_spec)
{
	if (pt_spec.inner != nullptr)
	{
		auto& type = get(pt_spec.inner);
		type.promote_pointer();
		RETURN(type);
	}

	Type t;
	t.promote_pointer();
	RETURN(std::move(t));
}

void TypeCreator::visit(BaseTypeSpec& bt_spec)
{
	if (inst_generics) instantiate_generic(bt_spec, scopes,ns); // instantiate already emits an error
	auto* bt = scopes.get_type(bt_spec.name.text);
	if (bt == nullptr)
	{
		//auto descr = Error::FromToken(bt_spec.name);
		//descr.Message = fmt::format("The type '{}' is undefined",bt_spec.name.text);
		//Error::SemanticError(descr);
		bt = &error_base_type;
		succ = false;
	}
	if (bt_spec.inner)
	{
		auto& inner = get(bt_spec.inner);
		inner.promote_base(bt);
		RETURN(inner);
	}
	RETURN(Type(bt));
}

void TypeCreator::visit(ArrayTypeSpec& at_spec)
{
	if (at_spec.inner != nullptr)
	{
		auto& type = get(at_spec.inner);
		type.promote_array(at_spec.amount->eval_res.val);
		RETURN(type);
	}

	Type t;
	t.promote_array(at_spec.amount->eval_res.val);
	RETURN(std::move(t));
}

void TypeCreator::visit(FptrTypeSpec& fptr)
{
	auto rettype = get(fptr.ret_type);
	ValuePtr<Type> ret_vp(new Type(rettype));
	
	std::vector<ValuePtr<Type>> value_ptrs;
	for (int i = 0; i < fptr.args.size(); i++)
	{
		auto& arg = get(fptr.args[i]);
		ValuePtr<Type> arg_t(new Type(arg));
		value_ptrs.push_back(arg_t);
	}
	RETURN(Type(std::move(ret_vp), std::move(value_ptrs)));
	//auto& ret = get(fptr.ret_type);
	//std::vector<
}




