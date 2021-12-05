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
	None
};

enum class PredefinedType
{
	Bool=0,
	UShort=1,
	UHalf=2,
	UChar=3,
	UInt=4,
	Char=5,
	Short=6,
	Half=7,
	Int=8,
	Float=9,
	Double=10,
	Quad=11,
	Void=12,
};

struct Type
{
	typedef std::variant<struct CollectionStmt*, struct PointerType, struct ArrayType, struct FunctionPointerType> TypeVariant;

	Type(CollectionStmt* bt);
	Type(uint64_t amount);
	Type(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	Type() = default;
	Type(const Type&) = default;
	Type& operator=(const Type&) = default;

	// E.g. int* == [(BaseType("int"),TypeKind::Base),(PointerType,TypeKind::Pointer)]
	std::vector<std::pair<TypeKind, TypeVariant>> stored_types;

	bool operator==(const Type& other) const;
	bool operator!=(const Type& other) const { return !(*this == other); };

	void promote_pointer();
	void promote_base(CollectionStmt* bt);
	void promote_array(uint64_t amount);
	void promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	void reverse();
	void pop();
	bool is_pointer_type() const;
	bool is_base_type() const;
	bool must_be_inferred() const;

	CollectionStmt* get_base_type() const;
	//PredefinedType pred_type;

	// Also account for cast operators later
	ConversionType get_conversion_into(const Type& other, const class Scopes& scopes);
	std::string as_str() const ;

	static std::pair<ConversionType, ConversionType> type_cast_to_more_general(PredefinedType t1, PredefinedType t2);
	static bool is_integer(PredefinedType pt);
	static bool is_unsigned_integer(PredefinedType pt);
	static bool is_signed_integer(PredefinedType pt);
};

struct ArrayType
{
	uint64_t amount;
};

struct FunctionPointerType
{
	ValuePtr<Type> return_type;
	std::vector<ValuePtr<Type>> args;
};

struct PointerType
{

};
extern CollectionStmt error_base_type;
extern Type error_type;