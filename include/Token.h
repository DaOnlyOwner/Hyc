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
	enum Type{
		IntegerU8, IntegerU16, IntegerU32, IntegerU64, IntegerS8, IntegerS16, IntegerS32, IntegerS64, Float, Double, Slash, Plus, Minus, Asterix, Eof, Error, Ident, Equal, Decl, Semicolon
	};

	Token(Type ttype, const std::string& text, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	Type type;
	std::string text;
	size_t lineNo;
	size_t colNo;
	size_t lineNoEnd;
	size_t colNoEnd;

	static std::string IntegerTypeToSuffixStr(Type ttype)
	{
		switch (ttype)
		{
		case Token::IntegerU8:
			return "u8";
		case Token::IntegerU16:
			return "u16";
		case Token::IntegerU32:
			return "u32";
		case Token::IntegerU64:
			return "u64";
		case Token::IntegerS8:
			return "s8";
		case Token::IntegerS16:
			return "s16";
		case Token::IntegerS32:
			return "s32";
		case Token::IntegerS64:
			return "s64";
		default:
			assert(false);
		}
	}

	static std::string Translate(Type ttype)
	{
		switch (ttype)
		{
		case IntegerU8:
			return "Integer (U8)";
		case IntegerU16:
			return "Integer (U16)";
		case IntegerU32:
			return "Integer (U32)";
		case IntegerU64:
			return "Integer (U64)";
		case IntegerS8:
			return "Integer (S8)";
		case IntegerS16:
			return "Integer (S16)";
		case IntegerS32:
			return "Integer (S32)";
		case IntegerS64:
			return "Integer (S64)";
		case Float:
			return "Float";
		case Double:
			return "Double";
		case Slash:
			return "Slash";
		case Plus:
			return "Plus";
		case Minus:
			return "Minus";
		case Asterix:
			return "Asterix";
		case Decl:
			return "DeclSign";  // :=
		case Ident:
			return "Identifier";
		case Equal:
			return "EqualSign"; // =
		case Semicolon:
			return ";";
		case Eof:
			return "EOF";

		case Error:
			return "Error";
		}
		return "Unknown";

	}

};



