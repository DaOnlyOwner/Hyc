#include <iostream>
#include "Lexer.h"
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
	std::ifstream fileInput(ROOT + std::string("/meta/debug.txt"));
	if (fileInput.bad())
	{
		printf("Couldn't open file");
		return -1;
	}
	Lexer lexer(fileInput);
	lexer.lex();
	auto tokens = lexer.get_tokens();
	for (int i = 0; i < tokens.size(); i++)
	{
		printf("%s\n", Token::Translate(tokens[i].type).c_str());
	}

	getchar();
}
