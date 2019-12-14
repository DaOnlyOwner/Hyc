#pragma once

#include "Lexer.h"
#include <fstream>

class Pipeline
{
public:
	Pipeline() = default;
	void build(std::ifstream& input);
};


