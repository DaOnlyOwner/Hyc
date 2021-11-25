#include "TypeCreator.h"
#include "DebugPrint.h"
#include "GenericInstantiation.h"
#include <cassert>
#include "ValuePtr.h"

std::pair<Type, bool> TypeCreator::create_type()
{
	auto type = get(spec);
	type.reverse();
	return { type,succ };
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
	instantiate_generic(bt_spec, scopes);
	auto* bt = scopes.get_base_type(bt_spec.as_str());
	if (bt == nullptr)
	{
		auto descr = Error::FromToken(bt_spec.name);
		descr.Message = fmt::format("The type '{}' is undefined",bt->name);
		Error::SemanticError(descr);
		bt = &error_base_type;
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
	uint64_t amount = at_spec.amount->eval_res.amount_uint;
	if(at_spec.inner)
	{
		auto& type = get(at_spec.inner);
		type.promote_array(amount);
		RETURN(type);
	}
	RETURN(Type(amount));
}

void TypeCreator::visit(FptrTypeSpec& fptr)
{
	auto ret = get(fptr.ret_type);
	ValuePtr<Type> ret_vp(new Type(ret));
	
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

std::pair<Type,bool> create_type(TypeSpec& ts, Scopes& scopes)
{
	TypeCreator tc(ts, scopes);
	return tc.create_type();
}




