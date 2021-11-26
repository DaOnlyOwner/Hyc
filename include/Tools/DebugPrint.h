#pragma once
#include <string>
#include "Token.h"
#include <stdexcept>

namespace Error
{
	class SyntaxErrorException : public std::exception
	{

	};
	struct ErrorDescription
	{
		std::string Message = "";
		size_t Line;
		size_t From;
		std::string Hint = "";
		std::string File;
	};
	extern bool Error;
	ErrorDescription FromToken(const Token& t);
	void Debug(const std::string& str);
	void SemanticError(const ErrorDescription& descr);
	void LexicalError(const ErrorDescription& descr);
	void SyntacticalError(const ErrorDescription& descr);
	void Info(const ErrorDescription& descr);
}