#include "Lexer.h"
#include "Parser.h"
#include "TerminalOutput.h"
#include <fstream>
#include <string>
#include "DebugPrint.h"
#include "DeclarationsCollector.h"
#include "TypeChecker.h"

int main()
{
	std::string filename = ROOT "/tests/test_input_parser_stmts.txt";
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

	TerminalOutput to;
	parsed->accept(to);

	//GraphOutput go;
	//parsed->accept(go);
	//DeclarationsCollector collector;
	//parsed->accept(collector);

	//TypeChecker tc(collector.get_scopes());
	//parsed->accept(tc);

	//go.write_to_file("go.dot");
}