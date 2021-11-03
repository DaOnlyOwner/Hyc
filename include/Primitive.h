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
		u8, u16, u32, u64,s8,s16,s32,s64,Float,Double,Count
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
	
private:
	Specifier m_specifier;
};