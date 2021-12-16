#pragma once

#include "Lexer.h"
#include <fstream>
#include "LLVMBackend.h"

class Pipeline
{
public:
	Pipeline() = default;
	int build(std::ifstream& input, const std::string& filename,const LLVMBackend::CompilerInfo& ci);
};


