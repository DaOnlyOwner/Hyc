#include "DebugPrint.h"
#include <cstdio>

void Debug(const std::string& str)
{
    printf("DEBUG: < %s >\n", str.c_str());
}

void SemanticError(const ErrorDescription& descr)
{
	std::string str = "===== Semantic Error =====\n" + descr.OffendingLine + "\n";
	for (int i = 0; i < descr.From; i++)
	{
		str += ' ';
	}
	for (int i = descr.From; i < descr.To; i++)
	{
		str += '~';
	}

	if(descr.Message != "") str += "\nMessage: " + descr.Message;
	if(descr.Hint != "") str += "\nHint: " + descr.Hint;
	str += "===== " + std::to_string(descr.From) + ", " + std::to_string(descr.To) + ", " + descr.File + "=====";
	fprintf(stderr, str.c_str());
}

void LexicalError(const ErrorDescription& descr)
{
}

void SyntacticalError(const ErrorDescription& descr)
{
}
