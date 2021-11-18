#pragma once
#include "Token.h"
#include "DebugPrint.h"

namespace Primitive
{
	enum class Specifier : size_t
	{
		u8=0, u16, u32, uint,s8,s16,s32,int_,Float,Double,Count
	};

	// TODO: Move to cpp file
	inline std::string Translate(Specifier spec)
	{
		switch (spec)
		{
		case Primitive::Specifier::u8:
			return "u8";
		case Primitive::Specifier::u16:
			return "u16";
		case Primitive::Specifier::u32:
			return "u32";
		case Primitive::Specifier::uint:
			return "uint";
		case Primitive::Specifier::s8:
			return "s8";
		case Primitive::Specifier::s16:
			return "s16";
		case Primitive::Specifier::s32:
			return "s32";
		case Primitive::Specifier::int_:
			return "int";
		case Primitive::Specifier::Float:
			return "float";
		case Primitive::Specifier::Double:
			return "double";
		case Primitive::Specifier::Count:
			return "Count";
		default:
			break;
		}
	}
}