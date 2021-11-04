#pragma once
#include "Type.h"
#include "Token.h"
#include "DebugPrint.h"

class Primitive : public Type
{
public:
	//TODO: Performance: To encode floating point, size and if it's signed or not, use a bitmap e.g. 1|0|bin(32)| means signed, not float, size==32. 
	enum class Specifier : size_t
	{
		u8=0, u16, u32, uint,s8,s16,s32,int_,Float,Double,Count
	};
	Primitive(const std::string& name, Specifier specifier)
		:Type(name),m_specifier(specifier){}

	Specifier get_type()
	{
		return m_specifier;
	}

	/*bool is_signed()
	{
		static_cast<unsigned int>(m_specifier) > 100;
	}

	virtual unsigned int get_size() override 
	{
		unsigned int size = static_cast<unsigned int>(m_specifier);
		return is_signed 
			? (size - 100 > 100 ?  size - 100 : size) 
			: size;
	}*/

	static std::string Translate(Specifier spec)
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
	
private:
	Specifier m_specifier;
};