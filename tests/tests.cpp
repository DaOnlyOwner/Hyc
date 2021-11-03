#include "Lexer.h"
#include "Parser.h"
#include "GraphOutput.h"
#include <fstream>
#include <string>
#include "DebugPrint.h"

int main()
{
	std::string filename = ROOT "/meta/debug.txt";
	std::ifstream fileInput(filename);
	reflex::Input lexerInput(fileInput);

	Lexer lexer(lexerInput);
	lexer.set_filename(filename);
	lexer.lex();

	Parser parser(lexer, filename);
	uptr<Stmt> parsed;
	try
	{
		parsed = parser.parse();
	}
	catch(Error::SyntaxErrorException se)
	{
		return -1;
	}

	GraphOutput go;
	parsed->accept(go);

	go.write_to_file("go.dot");
}