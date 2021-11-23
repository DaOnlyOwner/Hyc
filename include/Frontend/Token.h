#pragma once
#include <string>
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
		DoublePlus,
		Minus,
		DoubleMinus,
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
		DoubleColon,
		Less,
		Greater,
		DeclMv,
		Dot,
		Ampersand,
		DoubleAmpersand,
		Or,
		DoubleOr,
		MemAccess, // ->
		ExclMark,
		Tilde,
		Percent,
		ShiftLeft,
		ShiftRight,
		ThreeWay,
		LessEql,
		GreaterEql,
		DoubleEqual,
		NotEqual,
		Caret,
		QuestionMark,
		PlusEqual,
		MinusEqual,
		OrEqual,
		CaretEqual,
		AsterixEqual,
		SlashEqual,
		SlEqual,
		SrEqual,
		PercentEqual,
		AmpersandEqual,
		KwThrow,
		KwAs,
		KwStruct,
		KwNamespace,
		KwFor,
		KwWhile,
		KwIf,
		KwElif,
		KwReturn,
		KwElse,
		KwContinue,
		KwFptr,
		KwUnion,
		KwMatch,
		KwCase,
		KwOperator,
		KwAuto
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
		case Specifier::KwFor:
			return "for (keyword)";
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
		case Specifier::DoubleColon:
			return "::";
		case Specifier::DoublePlus:
			return "++";
		case Specifier::DoubleMinus:
			return "--";
		case Specifier::Dot:
			return ".";
		case Specifier::MemAccess:
			return "->";
		case Specifier::ExclMark:
			return "!";
		case Specifier::Tilde:
			return "~";
		case Specifier::KwAs:
			return "as (keyword)";
		case Specifier::Ampersand:
			return "&";
		case Specifier::Percent:
			return "%";
		case Specifier::ShiftLeft:
			return "<|";
		case Specifier::ShiftRight:
			return "|>";
		case Specifier::ThreeWay:
			return "<=>";
		case Specifier::LessEql:
			return "<=";
		case Specifier::GreaterEql:
			return ">=";
		case Specifier::DoubleEqual:
			return "==";
		case Specifier::NotEqual:
			return "!=";
		case Specifier::Caret:
			return "^";
		case Specifier::Or:
			return "|";
		case Specifier::DoubleAmpersand:
			return "&&";
		case Specifier::DoubleOr:
			return "||";
		case Specifier::QuestionMark:
			return "?";
		case Specifier::KwThrow:
			return "throw (keyword)";
		case Specifier::PlusEqual:
			return "+=";
		case Specifier::MinusEqual:
			return "-=";
		case Specifier::AsterixEqual:
			return "*=";
		case Specifier::SlashEqual:
			return "/=";
		case Specifier::PercentEqual:
			return "%=";
		case Specifier::SlEqual:
			return "<|=";
		case Specifier::SrEqual:
			return "|>=";
		case Specifier::AmpersandEqual:
			return "&=";
		case Specifier::CaretEqual:
			return "^=";
		case Specifier::OrEqual:
			return "|=";
		case Specifier::KwUnion:
			return "union (keyword)";
		case Specifier::KwMatch:
			return "match (keyword)";
		case Specifier::KwCase:
			return "case (keyword)";
		case Specifier::KwOperator:
			return "operator (keyword)";
		case Specifier::KwAuto:
			return "auto (keyword)";
		}
		return "Unknown";

	}

};



