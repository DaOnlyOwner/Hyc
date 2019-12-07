#pragma once
#include "StackCollection.h"
#include <vector>
#include <optional>

//std::optional<const Function&> InferType_FuncCall2Ary(const StackCollection& stack, const std::vector<Function>& functions /*overloads*/, const UID& typeLh, const UID& typeRh)
namespace TypeChecker
{
	// The question: Can I reach u2 from u1 or u1 from u2?
	UID Unify(const StackCollection& stack, const UID& u_, const UID& v_);
	bool IsUSubtypeV(const StackCollection& stack, const UID& u_, const UID& v_);
	std::optional<const Function&> InferType_FuncCall2Ary(const StackCollection& stack, const std::vector<Function>& functions, const UID& typeLh, const UID& typeRh);
}

