#include "TypeChecker.h"
#include "SymbolTable.h"
#include <algorithm>


bool wayFromUToV(const StackCollection& stack, const UID& u_, const UID& v_)
{
	if (u_ == v_) return true;
	const Struct& u = (const Struct&)stack.GetByUID(u_);
	const Struct& v = (const Struct&)stack.GetByUID(v_);
	for (int i = 0; i < u.InheritedTypes.size(); i++)
	{
		bool found = wayFromUToV(stack, u.InheritedTypes[i], v_);
		if (found)
		{
			return true;
		}
	}
	return false;
}

bool matchTypeToFunctionArgument(const StackCollection& stack, const std::vector<UID> types, const Function& function, int whichArgument)
{
	UID v = function.ArgumentTypeList[whichArgument];
	for (int i = 0; i < types.size(); i++)
	{
		UID u = types[i];
		if (TypeChecker::IsUSubtypeV(stack, u, v)) return true;
	}
	return false;
}

// Assumes there is no circle.
UID TypeChecker::Unify(const StackCollection& stack, const UID& u_, const UID& v_)
{
	if (wayFromUToV(stack, u_, v_)) return v_;
	else if (wayFromUToV(stack, v_, u_)) return u_;
	else return UID::Zero;
}

bool TypeChecker::IsUSubtypeV(const StackCollection& stack, const UID& u_, const UID& v_)
{
	return wayFromUToV(stack, u_, v_);
}

std::optional<const Function&> TypeChecker::InferType_FuncCall2Ary(const StackCollection& stack, const std::vector<Function>& functions /*overloads*/, const UID& typeLh, const UID& typeRh)
{
	for (const Function& func : functions)
	{
		bool b1 = TypeChecker::IsUSubtypeV(stack,typeLh, func.ArgumentTypeList[0]);
		if (!b1) continue;
		bool b2 = TypeChecker::IsUSubtypeV(stack, typeRh, func.ArgumentTypeList[1]);
		if (b2) return func;
	}
	return {};
}
