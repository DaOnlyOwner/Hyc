#pragma once
#include <string>
#include <cassert>
/*
	This file provides the datastructure of a token which is fed to the parser.
*/

struct Token
{
	// TODO: make this enum class.
	enum class Specifier {
		UInt,
		UHalf,
		UShort,
		UChar,
		Int,
		Half,
		Short,
		Char,
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
		Dot,
		Ampersand,
		DoubleAmpersand,
		Or,
		DoubleOr,
		MemAccess, // ->
		ExclMark,
		Tilde,
		Hashtag,
		Percent,
		ShiftLeft,
		ShiftRight,
		ThreeWay,
		LessEql,
		GreaterEql,
		DoubleEqual,
		NotEqual,
		OpSubs,
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
		KwAuto,
		GenFCallOpen,
		GenFCallClose,
		ArrayStart,
		ArrayEnd,
		Quad,
		DoubleQM,
		DoubleEM
	};

	Token(Specifier ttype, const std::string& text, const std::string& file, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),file(file),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	Token(Specifier ttype, const std::string& text, const std::string& file, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype), text(text), file(file), lineNo(lineNo), colNo(colNo), lineNoEnd(lineNoEnd), colNoEnd(colNoEnd){}

	Token(Specifier ttype, const std::string& text)
		:type(ttype),text(text),file(""),lineNo(0),colNo(0),lineNoEnd(0),colNoEnd(0){}

	Token() = default;

	Specifier type;
	std::string text;
	std::string file;
	size_t lineNo;
	size_t colNo;
	size_t lineNoEnd;
	size_t colNoEnd;


	static Specifier SplitCompound(Specifier compound)
	{
		switch (compound)
		{
		case Token::Specifier::PercentEqual:
			return Specifier::Percent;
		case Token::Specifier::CaretEqual:
			return Specifier::Caret;
		case Token::Specifier::SlEqual:
			return Specifier::ShiftLeft;
		case Token::Specifier::SrEqual:
			return Specifier::ShiftRight;
		case Token::Specifier::OrEqual:
			return Specifier::Or;
		case Token::Specifier::AmpersandEqual:
			return Specifier::Ampersand;
		case Token::Specifier::PlusEqual:
			return Specifier::Plus;
		case Token::Specifier::MinusEqual:
			return Specifier::Minus;
		case Token::Specifier::AsterixEqual:
			return Specifier::Asterix;
		case Token::Specifier::SlashEqual:
			return Specifier::Slash;
		default:
			assert(false);
			return (Specifier)0;
		}
	}

	static std::string Translate(Specifier ttype)
	{
		switch (ttype)
		{
		case Specifier::Int:
			return "Integer";
		case Specifier::UInt:
			return "Unsigned Integer";
		case Specifier::UHalf:
			return "Unsigned Half";
		case Specifier::UShort:
			return "Unsigned Short";
		case Specifier::UChar:
			return "Unsigned Char";
		case Specifier::Half:
			return "Half";
		case Specifier::Short:
			return "Short";
		case Specifier::Char:
			return "Char";
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
		case Specifier::Hashtag:
			return "#";
		case Specifier::OpSubs:
			return "operator []";
		case Specifier::GenFCallOpen:
			return "<.";
		case Specifier::GenFCallClose:
			return ".>";
		case Specifier::Quad:
			return "quad";
		case Specifier::ArrayEnd:
			return ".]";
		case Specifier::ArrayStart:
			return "[.";
		case Specifier::DoubleQM:
			return "??";
		case Specifier::DoubleEM:
			return "!!";
		}
		return "Unknown";

	}

};



