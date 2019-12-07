#pragma once
#include <string>

struct SemanticErrorDescription
{
	std::string Message;
	size_t Line;
	size_t From;
	size_t To;
	std::string Hint;
	std::string File;
	std::string OffendingLine;
};

void Debug(const std::string& str);
void SemanticError(const SemanticErrorDescription& descr);

