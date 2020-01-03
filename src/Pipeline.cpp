#include "Pipeline.h"
#include "Parser.h"
#include "GraphOutput.h"
#include "TypeChecker.h"
#include "DeclarationsCollector.h"

void Pipeline::build(std::ifstream& input)
{
	reflex::Input lexerInput(input);
	Lexer lexer(lexerInput);
	lexer.lex();
	
	Parser parser(lexer);
	auto parsed = parser.parse();

	GraphOutput go;
	parsed->accept(go);

	DeclarationsCollector dc;

	parsed->accept(dc);

	TypeChecker tc{ dc.get_scopes() };
	parsed->accept(tc);
	go.write_to_file("go.dot");
}
