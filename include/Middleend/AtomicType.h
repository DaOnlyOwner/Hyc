#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <memory>
#include <variant>
#include "ValuePtr.h"
#include <optional>
#include "TypeToLLVMType.h"

enum class TypeKind
{
	Pointer = 0,
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
	Bool = 0,
	UChar = 1,
	UShort = 2,
	UHalf = 3,
	UInt = 4,
	Char = 5,
	Short = 6,
	Half = 7,
	Int = 8,
	Float = 9,
	Double = 10,
	Quad = 11,
	Void = 12,
};

struct TypeSpec;
struct Type
{
	typedef std::variant<struct TypeDefStmt*, struct PointerType, struct ArrayType, struct FunctionPointerType> TypeVariant;

	Type(TypeDefStmt* bt);
	Type(uint64_t amount);
	Type(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	Type() { not_specified = true; }
	Type(const Type&) = default;
	Type& operator=(const Type&) = default;

	// E.g. int* == [(BaseType("int"),TypeKind::Base),(PointerType,TypeKind::Pointer)]
	std::vector<std::pair<TypeKind, TypeVariant>> stored_types;
	bool not_specified = false;

	bool operator==(const Type& other) const;
	bool operator!=(const Type& other) const { return !(*this == other); };

	void promote_pointer();
	void promote_base(TypeDefStmt* bt);
	void promote_array(uint64_t amount);
	void promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);

	Type with_pointer();
	Type with_base(TypeDefStmt* td);
	Type with_array(uint64_t amount);
	Type with_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args);
	Type with_pop();

	void reverse();
	void pop();
	bool is_pointer_type() const;
	bool is_base_type() const;
	bool is_fptr_type() const;
	bool is_array_type() const;
	bool is_error_type() const;
	bool is_void(Scopes& sc) const;
	bool must_be_inferred() const;
	std::pair<bool, Type> get_pointed_at() const;
	bool is_predefined(const class Scopes& sc) const;
	bool is_user_defined(const class Scopes& sc) const;
	std::optional<PredefinedType> to_pred(const class Scopes& sc) const;
	std::unique_ptr<TypeSpec> to_ast() const;


	TypeDefStmt* get_base_type() const;
	const FunctionPointerType* get_fptr() const;
	//PredefinedType pred_type;

	// Also account for cast operators later
	//ConversionType get_conversion_into(const Type& other, const class Scopes& scopes);
	std::string as_str() const;
	std::string as_str_for_mangling() const;


	static std::pair<ConversionType, ConversionType> type_cast_to_more_general(PredefinedType t1, PredefinedType t2);
	static bool is_integer(PredefinedType pt);
	static bool is_unsigned_integer(PredefinedType pt);
	static bool is_signed_integer(PredefinedType pt);
	static bool is_decimal(PredefinedType pt);

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
extern TypeDefStmt error_base_type;
extern Type error_type;