#pragma once
#include <string>
#include <cwchar>

/*
	This file provides the datastructure of a token which is fed to the parser.
*/

struct Token
{
	enum Type{
		Integer, Float, Slash, Plus, Minus, Asterix, Eof, Error, Ident, Equal, Decl, Semicolon
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

	static std::string Translate(Type ttype)
	{
		switch (ttype)
		{
		case Integer:
			return "Integer";
		case Float:
			return "Float";
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



