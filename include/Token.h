#pragma once
#include <string>
#include <cwchar>

struct Token
{
	enum TokenType{
		Integer, Float, Slash, Plus, Minus, Asterix, Eof, Error
	};

	Token(TokenType ttype, const std::string& text, const std::string& lineText, size_t lineNo, size_t colNo, size_t lineNoEnd, size_t colNoEnd)
		:type(ttype),text(text),lineNo(lineNo),colNo(colNo),lineNoEnd(lineNoEnd),colNoEnd(colNoEnd)
	{
		
	}

	TokenType type;
	std::string text;
	size_t lineNo;
	size_t colNo;
	size_t lineNoEnd;
	size_t colNoEnd;

	static std::string Translate(TokenType ttype)
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
		case Eof:
			return "EOF";

		case Error:
			return "Error";
		}
		return "Unknown";

	}

};



