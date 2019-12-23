#pragma once
#include <string>
#include <cwchar>
#include <cassert>

/*
	This file provides the datastructure of a token which is fed to the parser.
*/

struct Token
{
	// TODO: make this enum class.
	enum class Specifier{
		IntegerU8, 
		IntegerU16,
		IntegerU32,
		IntegerU64,
		IntegerS8,
		IntegerS16,
		IntegerS32,
		IntegerS64,
		Float,
		Double,
		Slash,
		Plus,
		Minus,
		Asterix,
		Eof,
		Error,
		Ident,
		Equal,
		Decl,
		Semicolon
	};

	Token(Specifier ttype, const std::string& text, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	Specifier type;
	std::string text;
	size_t lineNo;
	size_t colNo;
	size_t lineNoEnd;
	size_t colNoEnd;

	static std::string IntegerTypeToSuffixStr(Specifier ttype)
	{
		switch (ttype)
		{
		case Specifier::IntegerU8:
			return "u8";
		case Specifier::IntegerU16:
			return "u16";
		case Specifier::IntegerU32:
			return "u32";
		case Specifier::IntegerU64:
			return "u64";
		case Specifier::IntegerS8:
			return "s8";
		case Specifier::IntegerS16:
			return "s16";
		case Specifier::IntegerS32:
			return "s32";
		case Specifier::IntegerS64:
			return "s64";
		default:
			abort();
		}
	}

	static std::string Translate(Specifier ttype)
	{
		switch (ttype)
		{
		case Specifier::IntegerU8:
			return "Integer (U8)";
		case Specifier::IntegerU16:
			return "Integer (U16)";
		case Specifier::IntegerU32:
			return "Integer (U32)";
		case Specifier::IntegerU64:
			return "Integer (U64)";
		case Specifier::IntegerS8:
			return "Integer (S8)";
		case Specifier::IntegerS16:
			return "Integer (S16)";
		case Specifier::IntegerS32:
			return "Integer (S32)";
		case Specifier::IntegerS64:
			return "Integer (S64)";
		case Specifier::Float:
			return "Float";
		case Specifier::Double:
			return "Double";
		case Specifier::Slash:
			return "Slash";
		case Specifier::Plus:
			return "Plus";
		case Specifier::Minus:
			return "Minus";
		case Specifier::Asterix:
			return "Asterix";
		case Specifier::Decl:
			return "DeclSign";  // :=
		case Specifier::Ident:
			return "Identifier";
		case Specifier::Equal:
			return "EqualSign"; // =
		case Specifier::Semicolon:
			return ";";
		case Specifier::Eof:
			return "EOF";
		case Specifier::Error:
			return "Error";
		}
		return "Unknown";

	}

};



