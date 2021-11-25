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
	if (other.type_info.size() != type_info.size()) return false;
	for (int i = 0; i < type_info.size(); i++)
	{
		auto& [kind, acc] = type_info[i];
		auto& [okind, oacc] = other.type_info[i];
		if (kind != okind) return false;
		switch (kind)
		{
		case TypeKind::Pointer:
			break;
		case TypeKind::Base:
			if (std::get<BaseType>(acc).name != std::get<BaseType>(oacc).name) return false;
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
	std::reverse(type_info.begin(), type_info.end());
}

void Type::promote_pointer()
{
	type_info.push_back(std::make_pair(TypeKind::Pointer, TypeVariant{ PointerType{} }));
}

void Type::promote_base(BaseType* base)
{
	type_info.push_back(std::make_pair(TypeKind::Base, TypeVariant{ base }));
}

void Type::promote_array(uint64_t amount)
{
	type_info.push_back(std::make_pair(TypeKind::Array, TypeVariant(ArrayType{ amount })));
}

void Type::promote_fptr(ValuePtr<Type>&& ret, std::vector<ValuePtr<Type>>&& args)
{
	type_info.push_back(std::make_pair(TypeKind::FunctionPointer, TypeVariant(FunctionPointerType{ std::move(ret),std::move(args) })));
}

ConversionType Type::get_conversion_into(const Type& other, const Scopes& scopes)
{
	if (other.type_info.size() != type_info.size()) return ConversionType::NeedsCasting;
	ConversionType ctype = ConversionType::ImplicitCasting;
	for (int i = 0; i < type_info.size(); i++)
	{
		auto& [kind, acc] = type_info[i];
		auto& [okind, oacc] = other.type_info[i];
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
			const BaseType& bt = std::get<BaseType>(acc);
			const BaseType& obt = std::get<BaseType>(oacc);
			if (bt.name == obt.name)
			{
				ctype = ConversionType::Same;
			}
			else
			{
				bool contains = std::find(primitives.begin(), primitives.end(), bt.name) != primitives.end();
				bool ocontains = std::find(primitives.begin(), primitives.end(), obt.name) != primitives.end();
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
				ctype = ConversionType::Same;
			}
			break;
		}
	}
	return ctype;
}

std::string Type::as_str() const 
{
	std::string out = "";
	for (auto& [kind,var] : type_info)
	{
		switch (kind)
		{
		case TypeKind::Array:
			out += fmt::format("[{}]", std::get<ArrayType>(var).amount);
		case TypeKind::Base:
			out += std::get<BaseType>(var).name;
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

std::string Type::get_base_type() const
{
	for (auto& ti : type_info)
	{
		if (ti.first == TypeKind::Base) return std::get<BaseType>(ti.second).name;
	}

	assert(false);
	return "";
}

