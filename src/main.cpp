#include <iostream>
#include "antlr4/antlr4-runtime.h"
#include "ParserHelper.h"

int main()
{
	ParserHelper helper;
	auto* tree = helper.Parse(SRC_ROOT  + std::string("\\meta\\debug.txt"));
	getchar();

}
