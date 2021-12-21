// TODO: Rename to Error.h

#include "DebugPrint.h"
#include <cstdio>
#include "fmt/color.h"

void Error::Debug(const std::string& str)
{
    printf("DEBUG: < %s >\n", str.c_str());
}

namespace
{
	void print(const Error::ErrorDescription& descr, fmt::color c, const std::string& type)
	{
		std::string str = fmt::format("===== {} =====\nHere:{}:{}",type,descr.Line,descr.From);
		if (descr.Message != "") str += "\nMessage: " + descr.Message;
		if (descr.Hint != "") str += "\nHint: " + descr.Hint;
		str += fmt::format("\n===== {} =====",descr.File);
		fmt::print(fg(c), "{}\n\n",str);
	}
}

void Error::SemanticError(const ErrorDescription& descr)
{
	print(descr, fmt::color::orange_red, "Semantic Error");
	Error = true;
}

void Error::Info(const ErrorDescription& descr)
{
	print(descr, fmt::color::cadet_blue, "Info");
}

void Error::Warning(const ErrorDescription& descr)
{
	print(descr, fmt::color::orange, "Warning");
}

void Error::SyntacticalError(const ErrorDescription& descr)
{
	if (DoEmit)
	{
		print(descr, fmt::color::orange_red, "Syntax Error");
		Error = true;
	}
	throw SyntaxErrorException();
}

void Error::SetEmit(bool emit)
{
	DoEmit = emit;
}

Error::ErrorDescription Error::FromToken(const Token& t)
{
	ErrorDescription out;
	out.File = t.file;
	out.From = t.colNo;
	out.Line = t.lineNo;
	return out;
}

bool Error::Error = false;
bool Error::DoEmit = true;