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
		Integer,
		Float,
		Double,
		Slash,
		Plus,
		Minus,
		Asterix,
		Eof,
		Ident,
		Equal,
		Decl,
		Semicolon,
		kw_u8,
		kw_u16,
		kw_u32,
		kw_uint,
		kw_s8,
		kw_s16,
		kw_s32,
		kw_int,
		kw_float,
		kw_double,
		kw_return,
		comma,
		rparen_l,
		rparen_r,
		brace_l,
		brace_r
	};

	Token(Specifier ttype, const std::string& text, const std::string& file, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),file(file),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	Specifier type;
	std::string text;
	std::string file;
	size_t lineNo;
	size_t colNo;
	size_t lineNoEnd;
	size_t colNoEnd;


	static std::string Translate(Specifier ttype)
	{
		switch (ttype)
		{
		case Specifier::Integer:
			return "Integer";
		case Specifier::Float:
			return "Float";
		case Specifier::Double:
			return "Double";
		case Specifier::Slash:
			return "/";
		case Specifier::Plus:
			return "+";
		case Specifier::Minus:
			return "-";
		case Specifier::Asterix:
			return "*";
		case Specifier::Decl:
			return ":=";  // :=
		case Specifier::Ident:
			return "Identifier";
		case Specifier::Equal:
			return "="; // =
		case Specifier::Semicolon:
			return ";";
		case Specifier::Eof:
			return "EOF";
		case Specifier::kw_u8:
			return "u8 (keyword)";
		case Specifier::kw_u16:
			return "u16 (keyword)";
		case Specifier::kw_u32:
			return "u32 (keyword)";
		case Specifier::kw_uint:
			return "uint (keyword)";
		case Specifier::kw_s8:
			return "s8 (keyword)";
		case Specifier::kw_s16:
			return "s16 (keyword)";
		case Specifier::kw_s32:
			return "s32 (keyword)";
		case Specifier::kw_int:
			return "int (keyword)";
		case Specifier::kw_float:
			return "float (keyword)";
		case Specifier::kw_double:
			return "double (keyword)";
		case Specifier::comma:
			return ",";
		case Specifier::rparen_l:
			return ")";
		case Specifier::rparen_r:
			return "(";
		case Specifier::kw_return:
			return "return";
		case Specifier::brace_l:
			return "{";
		case Specifier::brace_r:
			return "}";
		}
		return "Unknown";

	}

};



