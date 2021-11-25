#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <memory>
#include <variant>
#include "ValuePtr.h"

enum class TypeKind
{
	Pointer=0,
	Array,
	FunctionPointer,
	Base
};

enum class ConversionType
{
	NeedsCasting,
	ImplicitCasting,
	Same
};

struct ArrayType
{
	uint64_t amount;
};

struct FunctionPointerType
{
	ValuePtr<struct Type> return_type;
	std::vector<ValuePtr<Type>> args;
	// TODO: Copy and default constructors etc.
};

struct BaseType
{
	std::string name;
};

struct PointerType
{

};

extern BaseType error_base_type;
extern Type error_type;

struct Type
{
	typedef std::variant<BaseType*, PointerType, ArrayType, FunctionPointerType> TypeVariant;

	Type(BaseType* bt);
	Type(uint64_t amount);
	Type(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	Type() = default;

	// E.g. int* == [(BaseType("int"),TypeKind::Base),(PointerType,TypeKind::Pointer)]
	std::vector<std::pair<TypeKind, TypeVariant>> type_info;

	bool operator==(const Type& other) const;
	bool operator!=(const Type& other) const { return !(*this == other); };

	void promote_pointer();
	void promote_base(BaseType* bt);
	void promote_array(uint64_t amount);
	void promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	void reverse();

	std::string get_base_type() const;

	// Also account for cast operators later
	ConversionType get_conversion_into(const Type& other, const class Scopes& scopes);
	std::string as_str() const ;
};
