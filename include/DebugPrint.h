#pragma once
#include <string>

struct ErrorDescription
{
	std::string Message = "";
	size_t Line;
	size_t From;
	size_t To;
	std::string Hint = "";
	std::string File;
	std::string OffendingLine;
};

void Debug(const std::string& str);
void SemanticError(const ErrorDescription& descr);
void LexicalError(const ErrorDescription& descr);
void SyntacticalError(const ErrorDescription& descr);