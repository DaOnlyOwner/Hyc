#include "AtomicType.h"
#include <array>
#include "fmt/core.h"
#include <algorithm>
#include <Scopes.h>
#include <cassert>

BaseType error_base_type{ "__error_type__" };
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
			if (std::get<BaseType*>(acc)->name != std::get<BaseType*>(oacc)->name) return false;
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

Type::Type(BaseType* base)
{
	promote_base(base);
}

Type::Type(uint64_t amount)
{
	promote_array(amount);
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
	return stored_types.back().first == TypeKind::Pointer;
}

bool Type::is_base_type() const
{
	return stored_types.back().first == TypeKind::Base;
}

void Type::promote_pointer()
{
	stored_types.push_back(std::make_pair(TypeKind::Pointer, TypeVariant{ PointerType{} }));
}

void Type::promote_base(BaseType* base)
{
	//if (base->name == "int") pred_type = PredefinedType::Int;
	//else if (base->name == "uint") pred_type = PredefinedType::UInt;
	//else if (base->name == "half") pred_type = PredefinedType::Half;
	//else if (base->name == "uhalf") pred_type = PredefinedType::UHalf;
	//else if (base->name == "short") pred_type = PredefinedType::Short;
	//else if (base->name == "ushort") pred_type = PredefinedType::UShort;
	//else if (base->name == "char") pred_type = PredefinedType::Char;
	//else if (base->name == "uchar") pred_type = PredefinedType::UChar;
	//else if (base->name == "bool") pred_type = PredefinedType::Bool;
	//else if (base->name == "float") pred_type = PredefinedType::Float;
	//else if (base->name == "double") pred_type = PredefinedType::Double;
	//else if (base->name == "quad") pred_type = PredefinedType::Quad;
	//else if (base->name == "void") pred_type == PredefinedType::Void;
	//else pred_type = PredefinedType::None;
	stored_types.push_back(std::make_pair(TypeKind::Base, TypeVariant{ base }));
}

void Type::promote_array(uint64_t amount)
{
	stored_types.push_back(std::make_pair(TypeKind::Array, TypeVariant(ArrayType{ amount })));
}

void Type::promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args)
{
	stored_types.push_back(std::make_pair(TypeKind::FunctionPointer, TypeVariant(FunctionPointerType{ std::move(ret),std::move(args) })));
}

ConversionType Type::get_conversion_into(const Type& other, const Scopes& scopes)
{
	if (other.stored_types.size() != stored_types.size()) return ConversionType::NeedsCasting;
	ConversionType ctype = ConversionType::ImplicitCasting;
	for (int i = 0; i < stored_types.size(); i++)
	{
		auto& [kind, acc] = stored_types[i];
		auto& [okind, oacc] = other.stored_types[i];
		if (kind != okind)
		{
			if (kind == TypeKind::Array && okind == TypeKind::Pointer) continue; // Array to pointer is implicitly casted
			return ConversionType::NeedsCasting;
		}
		switch (kind)
		{
		case TypeKind::Pointer:
			break;
		case TypeKind::Base:
		{
			std::array<std::string, (int)Primitive::Specifier::Count> primitives = { "u8","u16","u32","uint","s8","s16","s32","int","float","double" };
			BaseType* bt = std::get<BaseType*>(acc);
			BaseType* obt = std::get<BaseType*>(oacc);
			if (bt == obt)
			{
				ctype = ConversionType::None;
			}
			else
			{
				bool contains = std::find(primitives.begin(), primitives.end(), bt->name) != primitives.end();
				bool ocontains = std::find(primitives.begin(), primitives.end(), obt->name) != primitives.end();
				if (!(contains && ocontains)) return ConversionType::NeedsCasting;
			}
		}
		break;
		case TypeKind::Array:
			if(std::get<ArrayType>(acc).amount != std::get<ArrayType>(oacc).amount) return ConversionType::NeedsCasting;
			break;
		// Function pointers need to be exact
		case TypeKind::FunctionPointer:
			auto& fp = std::get<FunctionPointerType>(acc);
			auto& ofp = std::get<FunctionPointerType>(oacc);
			if (fp.args.size() != ofp.args.size()) return ConversionType::NeedsCasting;
			if (*fp.return_type != *ofp.return_type) return ConversionType::NeedsCasting;
			for (int j = 0; j < fp.args.size(); j++)
			{
				Type& t = *fp.args[j];
				Type& ot = *ofp.args[j];
				if (t != ot) return ConversionType::NeedsCasting;
				ctype = ConversionType::None;
			}
			break;
		}
	}
	return ctype;
}

std::string Type::as_str() const 
{
	std::string out = "";
	for (auto& [kind,var] : stored_types)
	{
		switch (kind)
		{
		case TypeKind::Array:
			out += fmt::format("[{}]", std::get<ArrayType>(var).amount);
		case TypeKind::Base:
			out += std::get<BaseType*>(var)->name;
		case TypeKind::Pointer:
			out += "*";
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
		}
	}
	return out;
}

std::pair<ConversionType, ConversionType> Type::type_cast_to_more_general(PredefinedType t1, PredefinedType t2)
{
	int wt1 = (int)t1;
	int wt2 = (int)t2;
	if (wt1 < wt2) return std::make_pair(ConversionType::ImplicitCasting, ConversionType::None);
	else if (wt1 > wt2) return std::make_pair(ConversionType::None, ConversionType::ImplicitCasting);
	else return std::make_pair(ConversionType::None, ConversionType::None);
}

bool Type::is_numeric(PredefinedType pt)
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

BaseType* Type::get_base_type() const
{
	for (auto& ti : stored_types)
	{
		if (ti.first == TypeKind::Base) return std::get<BaseType*>(ti.second);
	}

	assert(false);
	return nullptr;
}

