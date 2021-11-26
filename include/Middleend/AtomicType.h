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



struct Type
{
	typedef std::variant<struct BaseType*, struct PointerType, struct ArrayType, struct FunctionPointerType> TypeVariant;

	Type(BaseType* bt);
	Type(uint64_t amount);
	Type(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	Type() = default;
	Type(const Type&) = default;
	Type& operator=(const Type&) = default;

	// E.g. int* == [(BaseType("int"),TypeKind::Base),(PointerType,TypeKind::Pointer)]
	std::vector<std::pair<TypeKind, TypeVariant>> type_info;

	bool operator==(const Type& other) const;
	bool operator!=(const Type& other) const { return !(*this == other); };

	void promote_pointer();
	void promote_base(BaseType* bt);
	void promote_array(uint64_t amount);
	void promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	void reverse();

	BaseType* get_base_type() const;

	// Also account for cast operators later
	ConversionType get_conversion_into(const Type& other, const class Scopes& scopes);
	std::string as_str() const ;
};

extern BaseType error_base_type;
extern Type error_type;

struct ArrayType
{
	uint64_t amount;
};

struct FunctionPointerType
{
	ValuePtr<Type> return_type;
	std::vector<ValuePtr<Type>> args;
};

struct BaseType
{
	std::string name;
};

struct PointerType
{

};