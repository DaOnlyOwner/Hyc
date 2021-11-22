#include "Operators.h"
#include <cassert>

std::string TokenToOperatorStr(const Token& t)
{
	return "operator" + t.text;
}

std::string BinOp::Translate(BinOp::Specifier spec)
{
	switch (spec)
	{
	case BinOp::Specifier::Minus:
		return "operator+";
	case BinOp::Specifier::Plus:
		return "operator-";
	default:
		assert(false);
		return "unsupported";
	}
}

std::string UnaryOp::Translate(UnaryOp::Specifier spec)
{
	switch (spec)
	{
	case UnaryOp::Specifier::Minus:
		return "operator+";
	case UnaryOp::Specifier::Plus:
		return "operator-";
	default:
		assert(false);
		return "unsupported";
	}
}
