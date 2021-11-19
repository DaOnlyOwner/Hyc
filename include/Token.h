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
		DeclCpy,
		Semicolon,
		Comma,
		RParenL,
		RParenR,
		BraceL,
		BraceR,
		BracketL,
		BracketR,
		Colon,
		Less,
		Greater,
		DeclMv,
		KwStruct,
		KwNamespace,
		KwContract,
		KwFulfill,
		KwFor,
		KwWhile,
		KwIf,
		KwElif,
		KwReturn,
		KwElse,
		KwContinue,
		KwFptr
	};

	Token(Specifier ttype, const std::string& text, const std::string& file, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),file(file),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	Token() = default;

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
		case Specifier::DeclCpy:
			return ":=";  // :=
		case Specifier::Ident:
			return "Identifier";
		case Specifier::Equal:
			return "="; // =
		case Specifier::Semicolon:
			return ";";
		case Specifier::Eof:
			return "EOF";
		case Specifier::Comma:
			return ",";
		case Specifier::RParenL:
			return "(";
		case Specifier::RParenR:
			return ")";
		case Specifier::KwReturn:
			return "return (keyword)";
		case Specifier::BraceL:
			return "{";
		case Specifier::BraceR:
			return "}";
		case Specifier::BracketL:
			return "[";
		case Specifier::BracketR:
			return "]";
		case Specifier::KwIf:
			return "if (keyword)";
		case Specifier::KwElif:
			return "elif (keyword)";
		case Specifier::KwElse:
			return "else (keyword)";
		case Specifier::KwContract:
			return "contract (keyword)";
		case Specifier::KwFor:
			return "for (keyword)";
		case Specifier::KwFulfill:
			return "fulfill (keyword)";
		case Specifier::KwNamespace:
			return "namespace (keyword)";
		case Specifier::KwStruct:
			return "struct (keyword)";
		case Specifier::KwWhile:
			return "while (keyword)";
		case Specifier::Less:
			return "<";
		case Specifier::Greater:
			return ">";
		case Specifier::DeclMv:
			return ":#";
		case Specifier::Colon:
			return ":";
		case Specifier::KwContinue:
			return "continue (keyword)";
		case Specifier::KwFptr:
			return "fptr (keyword)";
		}
		return "Unknown";

	}

};



