#include "Pipeline.h"
#include "Parser.h"
#include "GraphOutput.h"
#include "TypeChecker.h"

void Pipeline::build(std::ifstream& input)
{
	reflex::Input lexerInput(input);
	Lexer lexer(lexerInput);
	lexer.lex();
	
	Parser parser(lexer);
	auto parsed = parser.parse();

	TypeChecker tc;
	parsed->accept(tc);
	

	//go.write_to_file("test.dot");

}
