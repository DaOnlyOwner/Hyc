#include "AtomicType.h"
#include <array>
#include "fmt/core.h"
#include <algorithm>
#include <Scopes.h>
#include <cassert>
#include "Ast.h"
#include "Scopes.h"
#include "Messages.h"

TypeDefStmt error_base_type{ "__error_type__" };
Type error_type{ &error_base_type };

bool Type::operator==(const Type& other) const
{
	if (other.stored_types.size() != stored_types.size()) return false;
	for (int i = 0; i < stored_types.size(); i++)
	{
		auto& [kind, acc] = stored_types[i];
		auto& [okind, oacc] = other.stored_types[i];
		if (kind != okind) return false;
		switch (kind)
		{
		case TypeKind::Pointer:
			break;
		case TypeKind::Base:
			if (std::get<TypeDefStmt*>(acc)->name.text != std::get<TypeDefStmt*>(oacc)->name.text) return false;
			break;
		case TypeKind::Array:
			if (std::get<ArrayType>(acc).amount != std::get<ArrayType>(oacc).amount) return false;
			break;
		case TypeKind::FunctionPointer:
			auto& fp = std::get<FunctionPointerType>(acc);
			auto& ofp = std::get<FunctionPointerType>(oacc);
			if (fp.args.size() != ofp.args.size()) return false;
			if (*fp.return_type != *ofp.return_type) return false;
			for (int j = 0; j < fp.args.size(); j++)
			{
				Type& t = *fp.args[j];
				Type& ot = *ofp.args[j];
				if (t != ot) return false;
			}
			break;
		}
	}
	return true;
}

Type::Type(TypeDefStmt* base)
{
	promote_base(base);
}

Type::Type(uint64_t amount)
{
	promote_array(amount);
}

bool Type::is_predefined(const Scopes& sc) const
{
	return is_base_type() && sc.is_type_predefined(get_base_type());
}

bool Type::is_user_defined(const Scopes& sc) const
{
	return !is_predefined(sc) && is_base_type();
}

std::optional<PredefinedType> Type::to_pred(const Scopes& sc) const
{
	if (!is_predefined(sc)) return {};
	return std::make_optional(sc.get_predefined_type(get_base_type()));
}


Type::Type(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args)
{
	promote_fptr(std::move(ret), std::move(args));
}

void Type::reverse()
{
	std::reverse(stored_types.begin(), stored_types.end());
}

bool Type::is_pointer_type() const
{
	return !not_specified && stored_types.back().first == TypeKind::Pointer;
}

bool Type::is_base_type() const
{
	return !not_specified && stored_types.back().first == TypeKind::Base;
}

bool Type::must_be_inferred() const { return not_specified; }

std::pair<bool, Type> Type::get_pointed_at() const
{
	if (!is_pointer_type()) return { false,Type() };
	auto cpy = *this;
	cpy.pop();
	return { true,cpy };
}

void Type::promote_pointer()
{
	not_specified = false;
	stored_types.push_back(std::make_pair(TypeKind::Pointer, TypeVariant{ PointerType{} }));
}

void Type::promote_base(TypeDefStmt* base)
{
	not_specified = false;
	stored_types.push_back(std::make_pair(TypeKind::Base, TypeVariant{ base }));
}

void Type::promote_array(uint64_t amount)
{
	not_specified = false;
	stored_types.push_back(std::make_pair(TypeKind::Array, TypeVariant(ArrayType{ amount })));
}

void Type::promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args)
{
	not_specified = false;
	stored_types.push_back(std::make_pair(TypeKind::FunctionPointer, TypeVariant(FunctionPointerType{ std::move(ret),std::move(args) })));
}

Type Type::with_pointer()
{
	Type t(*this);
	t.promote_pointer();
	return t;
}

Type Type::with_base(TypeDefStmt* td)
{
	Type t = *this;
	t.promote_base(td);
	return t;
}

Type Type::with_array(uint64_t amount)
{
	Type t = *this;
	t.promote_array(amount);
	return t;
}

Type Type::with_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args)
{
	Type t = *this;
	t.promote_fptr(std::move(ret), std::move(args));
	return t;
}

Type Type::with_pop()
{
	auto t = *this;
	t.pop();
	return t;
}

std::string Type::as_str() const
{
	std::string out = "";
	for (auto& [kind, var] : stored_types)
	{
		switch (kind)
		{
		case TypeKind::Array:
			out += fmt::format("[{}]", std::get<ArrayType>(var).amount);
			break;
		case TypeKind::Base:
			out += std::get<TypeDefStmt*>(var)->name.text;
			break;
		case TypeKind::Pointer:
			out += "*";
			break;
		case TypeKind::FunctionPointer:
			const FunctionPointerType& fp = std::get<FunctionPointerType>(var);
			std::string args = "";
			if (fp.args.size() > 0)
			{
				auto& arg_first = fp.args[0];
				args += arg_first->as_str();
				for (int i = 1; i < fp.args.size(); i++)
				{
					auto& arg = fp.args[i];
					args += fmt::format(", {}", arg->as_str());
				}
			}
			out += fmt::format("fptr({};{})", args, fp.return_type->as_str());
			break;
		}
	}
	return out;
}

std::string Type::as_str_for_mangling() const
{
	std::string out = "";
	for (auto& [kind, var] : stored_types)
	{
		switch (kind)
		{
		case TypeKind::Array:
			out += fmt::format("[{}]", std::get<ArrayType>(var).amount); // No array in function
			break;
		case TypeKind::Base:
			out += std::get<TypeDefStmt*>(var)->name.text;
			break;
		case TypeKind::Pointer:
			out += "$";
			break;
		case TypeKind::FunctionPointer:
			const FunctionPointerType& fp = std::get<FunctionPointerType>(var);
			std::string args = "";
			if (fp.args.size() > 0)
			{
				auto& arg_first = fp.args[0];
				args += arg_first->as_str_for_mangling();
				for (int i = 1; i < fp.args.size(); i++)
				{
					auto& arg = fp.args[i];
					args += fmt::format("{}", arg->as_str_for_mangling());
				}
			}
			out += fmt::format("%fptr{}_{}%", args, fp.return_type->as_str_for_mangling());
			break;
		}
	}
	return out;
}

std::unique_ptr<TypeSpec> Type::to_ast() const
{
	std::unique_ptr<TypeSpec> to_build;

	assert(!must_be_inferred());

	//assert(is_fptr_type() || is_base_type());

	for (int i = 0; i < stored_types.size(); i++)
	{
		auto& st = stored_types[i];
		if (st.first == TypeKind::Base)
		{
			auto coll = std::get<TypeDefStmt*>(st.second);
			to_build = std::make_unique<BaseTypeSpec>(Token(coll->name));
		}

		else if (st.first == TypeKind::FunctionPointer)
		{
			auto& fptr = std::get<FunctionPointerType>(st.second);
			std::vector<uptr<TypeSpec>> args;
			for (auto& arg : fptr.args)
			{
				args.push_back(arg->to_ast());
			}
			auto ret_type = fptr.return_type->to_ast();
			to_build = std::make_unique<FptrTypeSpec>(std::move(args), std::move(ret_type), nullptr);
		}
		else if (st.first == TypeKind::Pointer)
		{
			to_build->push_inner(std::make_unique<PointerTypeSpec>(nullptr));
		}

		else if (st.first == TypeKind::Array)
		{
			/*uint64_t amount = std::get<ArrayType>(st.second).amount;
			Token tk(Token::Specifier::, std::to_string(amount));
			EvalIntegerResult eir;
			eir.Spec = Primitive::Specifier::uint;
			eir.amount_uint = amount;
			auto ile = std::make_unique<IntegerLiteralExpr>(tk, eir);
			to_build->push_inner(std::make_unique<ArrayTypeSpec>(std::move(ile), nullptr));*/
			NOT_IMPLEMENTED;
		}

	}

	return std::move(to_build);

}

bool Type::is_fptr_type() const
{
	return !not_specified && stored_types.back().first == TypeKind::FunctionPointer;
}

bool Type::is_array_type() const
{
	return !not_specified && stored_types.back().first == TypeKind::Array;
}

bool Type::is_error_type() const
{
	return *this == error_type;
}

bool Type::is_void(Scopes& sc) const
{
	return is_base_type() && (get_base_type() == sc.get_type("void"));
}


std::pair<ConversionType, ConversionType> Type::type_cast_to_more_general(PredefinedType t1, PredefinedType t2)
{
	int wt1 = (int)t1;
	int wt2 = (int)t2;
	if (wt1 < wt2) return std::make_pair(ConversionType::ImplicitCasting, ConversionType::None);
	else if (wt1 > wt2) return std::make_pair(ConversionType::None, ConversionType::ImplicitCasting);
	else return std::make_pair(ConversionType::None, ConversionType::None);
}

void Type::pop()
{
	stored_types.pop_back();
}

bool Type::is_integer(PredefinedType pt)
{
	switch (pt)
	{
	case PredefinedType::Int:
		return true;
	case PredefinedType::UInt:
		return true;
	case PredefinedType::Half:
		return true;
	case PredefinedType::UHalf:
		return true;
	case PredefinedType::Short:
		return true;
	case PredefinedType::UShort:
		return true;
	case PredefinedType::Char:
		return true;
	case PredefinedType::UChar:
		return true;
	case PredefinedType::Bool:
		return true;
	case PredefinedType::Float:
		return false;
	case PredefinedType::Double:
		return false;
	case PredefinedType::Quad:
		return false;
	case PredefinedType::Void:
		return false;
	default:
		break;
	}
}

bool Type::is_unsigned_integer(PredefinedType pt)
{
	switch (pt)
	{
	case PredefinedType::Bool:
		return false;
	case PredefinedType::UShort:
		return true;
	case PredefinedType::UHalf:
		return true;
	case PredefinedType::UChar:
		return true;
	case PredefinedType::UInt:
		return true;
	case PredefinedType::Char:
		return false;
	case PredefinedType::Short:
		return false;
	case PredefinedType::Half:
		return false;
	case PredefinedType::Int:
		return false;
	case PredefinedType::Float:
		return false;
	case PredefinedType::Double:
		return false;
	case PredefinedType::Quad:
		return false;
	case PredefinedType::Void:
		return false;
	default:
		assert(false);
	}
}

bool Type::is_signed_integer(PredefinedType pt)
{
	switch (pt)
	{
	case PredefinedType::Bool:
		return false;
	case PredefinedType::UShort:
		return false;
	case PredefinedType::UHalf:
		return false;
	case PredefinedType::UChar:
		return false;
	case PredefinedType::UInt:
		return false;
	case PredefinedType::Char:
		return true;
	case PredefinedType::Short:
		return true;
	case PredefinedType::Half:
		return true;
	case PredefinedType::Int:
		return true;
	case PredefinedType::Float:
		return false;
	case PredefinedType::Double:
		return false;
	case PredefinedType::Quad:
		return false;
	case PredefinedType::Void:
		return false;
	default:
		assert(false);
	}
}

bool Type::is_decimal(PredefinedType pt)
{
	return pt == PredefinedType::Float
		|| pt == PredefinedType::Double
		|| pt == PredefinedType::Quad;
}


TypeDefStmt* Type::get_base_type() const
{
	if (not_specified) return nullptr;
	for (auto& ti : stored_types)
	{
		if (ti.first == TypeKind::Base) return std::get<TypeDefStmt*>(ti.second);
	}

	assert(false);
	return nullptr;
}

const FunctionPointerType* Type::get_fptr() const
{
	if (not_specified) return nullptr;
	for (auto& ti : stored_types)
	{
		if (ti.first == TypeKind::FunctionPointer)
		{
			const FunctionPointerType& fptr = std::get<FunctionPointerType>(ti.second);
			return &fptr;
		}
	}
	assert(false);
	return nullptr;
}
