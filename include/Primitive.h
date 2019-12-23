#pragma once
#include "MetaType.h"
#include "Token.h"
#include "DebugPrint.h"

class Primitive : public MetaType
{
public:
	//TODO: Performance: To encode floating point, size and if it's signed or not, use a bitmap e.g. 1|0|bin(32)| means signed, not float, size==32. 
	enum class Specifier : size_t
	{
		u8, u16, u32, u64,s8,s16,s32,s64,Float,Double,Count
	};
	Primitive(const std::string& name, Specifier specifier)
		:MetaType(name),m_specifier(specifier){}

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
	
	static Specifier from_token_specifier(Token::Specifier ttype)
	{
		switch (ttype)
		{
		case Token::Specifier::IntegerU8:
			return Specifier::u8;
		case Token::Specifier::IntegerU16:
			return Specifier::u16;
		case Token::Specifier::IntegerU32:
			return Specifier::u32;
		case Token::Specifier::IntegerU64:
			return Specifier::u64;
		case Token::Specifier::IntegerS8:
			return Specifier::s8;
		case Token::Specifier::IntegerS16:
			return Specifier::s16;
		case Token::Specifier::IntegerS32:
			return Specifier::s32;
		case Token::Specifier::IntegerS64:
			return Specifier::s64;
		default:
			Debug("Bad primitive from token type. Compilerbug");
			abort();
		}
	}

	IMPL_VISITOR_TYPE
private:
	Specifier m_specifier;
};