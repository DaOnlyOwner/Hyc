#define CATCH_CONFIG_MAIN
#include "Lexer.h"
#include "Parser.h"
#include "TerminalOutput.h"
#include <fstream>
#include <string>
#include "catch.hpp"

namespace
{
	std::string parse(const std::string& filename)
	{
		std::ifstream fileInput(filename);
		if (!fileInput) return "Error-wrong filename";
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
		catch (Error::SyntaxErrorException se)
		{
			return "Error-parsing";
		}

		TerminalOutput to;
		parsed->accept(to);
		return to.get_format_str();
	}

	std::string read_in(const std::string& filename)
	{
		std::ifstream fileInput(filename);
		if (!fileInput) return "";
		std::stringstream buffer;
		buffer << fileInput.rdbuf();
		return buffer.str();
	}

	void write_to(const std::string& filename, const std::string& to_write)
	{
		std::ofstream f(filename);
		f << to_write;
	}

}

TEST_CASE("DEBUG") 
{
	std::string filename_exprs = ROOT "/tests/test_input_parser_exprs.txt";
	std::string filename_stmts = ROOT "/tests/test_input_parser_stmts.txt";
	auto out_expr = parse(filename_exprs);
	auto out_stmt = parse(filename_stmts);
	fmt::print("{}\n\n", out_expr);
	fmt::print("{}", out_stmt);
	//write_to(ROOT "/tests/required_output_parser_exprs.txt", out_expr);
	//write_to(ROOT "/tests/required_output_parser_stmts.txt", out_stmt);
}

TEST_CASE("Parser Stmts")
{
	auto out = parse(ROOT "/tests/test_input_parser_stmts.txt");
	auto req = read_in(ROOT "/tests/required_output_parser_stmts.txt");
	REQUIRE(out == req);
}

TEST_CASE("Parser Exprs")
{
	auto out = parse(ROOT "/tests/test_input_parser_exprs.txt");
	auto req = read_in(ROOT "/tests/required_output_parser_exprs.txt");
	REQUIRE(req == out);
}
