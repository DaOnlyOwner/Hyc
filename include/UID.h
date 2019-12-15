#pragma once

#include <string>
#include <memory>

struct UID
{
	UID(size_t stack_index, size_t index)
		:stack_index(stack_index),index(index){}
	UID() = default;
	bool operator==(const UID& other) const	
	{	
		return other.stack_index == stack_index && other.index == index;
	}

	bool operator!=(const UID& other) const
	{
		return !(*this == other);
	}

	unsigned int stack_index = 0;
	unsigned int index = 0;

	static const UID Zero;
};
