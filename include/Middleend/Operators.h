#pragma once
#include <string>
#include "Token.h"

std::string TokenToOperatorStr(const Token& t);

namespace BinOp
{
	enum class Specifier
	{
		Minus,
		Plus,
		Count
	};

	std::string Translate(Specifier spec);
}

namespace UnaryOp
{
	enum class Specifier
	{
		Minus, 
		Plus,
		Count
	};

	std::string Translate(Specifier spec);
}

