#pragma once

#include <string>
#include <memory>


struct UID
{
	bool operator==(const UID& other) const
	{
		return other.Type == Type && other.StackIndex == StackIndex && other.Index == Index;
	}

	bool operator!=(const UID& other) const
	{
		return !(*this == other);
	}

	enum TypeSpecifier
	{
		None, Struct, Function
	};

	TypeSpecifier Type;
	
	unsigned int StackIndex;
	unsigned int Index;

	static const UID Zero;
};

struct Type
{
	std::string Name;
};

struct Variable
{
	std::string Name;
	UID Uid;
};

// Not a definable type, but a type
struct Function : Type
{
	std::vector<UID> ArgumentTypeList;
	UID ReturnType;
};

struct Struct : Type
{
	std::vector<UID> InheritedTypes;
	std::vector<UID> EmbeddedTypes;
};







