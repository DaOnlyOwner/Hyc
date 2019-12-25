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

	GraphOutput go;
	parsed->accept(go);
	go.write_to_file("go.dot");

	TypeChecker tc;
	parsed->accept(tc);
	
	Scopes scopes = tc.get_scopes();
	scopes.debug_print();

	//go.write_to_file("test.dot");

}
