#include "Pipeline.h"
#include "Parser.h"
#include "GraphOutput.h"

void Pipeline::build(std::ifstream& input)
{
	reflex::Input lexerInput(input);
	Lexer lexer(lexerInput);
	lexer.lex();
	
	Parser parser(lexer);
	auto parsed = parser.parse();

	GraphOutput go;
	parsed->accept(go);
	go.write_to_file("test.dot");

}
