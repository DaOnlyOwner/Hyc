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
	enum class Specifier {
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
		Semicolon,
		kw_u8,
		kw_u16,
		kw_u32,
		kw_u64,
		kw_s8,
		kw_s16,
		kw_s32,
		kw_s64,
		kw_float,
		kw_double,
		kw_return,
		comma,
		rparen_l,
		rparen_r,
		brace_l,
		brace_r
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
		case Specifier::kw_u8:
			return "u8 (keyword)";
		case Specifier::kw_u16:
			return "u16 (keyword)";
		case Specifier::kw_u32:
			return "u32 (keyword)";
		case Specifier::kw_u64:
			return "u64 (keyword)";
		case Specifier::kw_s8:
			return "s8 (keyword)";
		case Specifier::kw_s16:
			return "s16 (keyword)";
		case Specifier::kw_s32:
			return "s32 (keyword)";
		case Specifier::kw_s64:
			return "s64 (keyword)";
		case Specifier::kw_float:
			return "float (keyword)";
		case Specifier::kw_double:
			return "kw_double";
		case Specifier::comma:
			return "Comma ','";
		case Specifier::rparen_l:
			return "Paren round, right ')'";
		case Specifier::rparen_r:
			return "Paren round, left '('";
		case Specifier::kw_return:
			return "kw_return";
		case Specifier::brace_l:
			return "brace_l";
		case Specifier::brace_r:
			return "brace_r";
		}
		return "Unknown";

	}

};



