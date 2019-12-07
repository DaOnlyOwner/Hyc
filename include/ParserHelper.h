#pragma once
#include "antlr4/antlr4-runtime.h"


class ParserHelper
{
public:
    ParserHelper();
    antlr4::tree::ParseTree* Parse(const std::string& filename);
};
