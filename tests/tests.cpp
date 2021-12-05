#define CATCH_CONFIG_MAIN
#include "Lexer.h"
#include "Parser.h"
#include "TerminalOutput.h"
#include <fstream>
#include <string>
#include "catch.hpp"

#include "ExpandScopes.h"
#include "DeclarationsCollectorTypes.h"
#include "DeclarationsCollectorFunctions.h"
#include "CreateFuncArgsType.h"
#include "GenericInstantiation.h"
#include "DefaultTypeArgChecker.h"
#include "Desugar.h"
#include "MemberCollector.h"
#include "TypeChecker.h"

namespace
{
	std::pair<std::string,uptr<NamespaceStmt>> parse(const std::string& filename)
	{
		std::ifstream fileInput(filename);
		if (!fileInput) return { "Error-wrong filename",nullptr };
		reflex::Input lexerInput(fileInput);

		Lexer lexer(lexerInput);
		lexer.set_filename(filename);
		lexer.lex();

		Parser parser(lexer, filename);
		uptr<NamespaceStmt> parsed;
		try
		{
			parsed = parser.parse();
		}
		catch (Error::SyntaxErrorException se)
		{
			return { "Error-parsing",nullptr };
		}

		TerminalOutput to;
		parsed->accept(to);
		return { to.get_format_str(),std::move(parsed) };
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
	//std::string filename_exprs = ROOT "/tests/test_input_parser_exprs.txt";
	//std::string filename_stmts = ROOT "/tests/test_input_parser_stmts.txt";
	//auto [out_expr,_1] = parse(filename_exprs);
	//auto [out_stmt,_2] = parse(filename_stmts);
	//fmt::print("{}\n\n", out_expr);
	//fmt::print("{}", out_stmt);
	//write_to(ROOT "/tests/required_output_parser_exprs.txt", out_expr);
	//write_to(ROOT "/tests/required_output_parser_stmts.txt", out_stmt);

	std::string filename_coll = ROOT "/tests/test_input_desugar.txt";
	auto [_3, parsed] = parse(filename_coll);
	Scopes sc;
	expand_scopes(*parsed, sc);
	desugar(*parsed);
	collect_types(*parsed,sc);
	check_default_type_arg(*parsed, sc);
	create_func_args_type(sc, *parsed);
	bool changed = true;
	// Loop as long as the AST keeps expanding (new types instantiated from generics)
	GenericInst gi(sc, *parsed);
	size_t n = 0;
	while (parsed->stmts.size() > n)
	{
		n = parsed->stmts.size();
		parsed->accept(gi);
	}
	collect_funcs(*parsed, sc);
	collect_members(*parsed, sc);
	check_type(*parsed, sc);
	TerminalOutput to;
	parsed->accept(to);
	fmt::print("{}", to.get_format_str());
}

TEST_CASE("Parser Stmts")
{
	auto [out,_] = parse(ROOT "/tests/test_input_parser_stmts.txt");
	auto req = read_in(ROOT "/tests/required_output_parser_stmts.txt");
	REQUIRE(out == req);
}

TEST_CASE("Parser Exprs")
{
	auto [out,_] = parse(ROOT "/tests/test_input_parser_exprs.txt");
	auto req = read_in(ROOT "/tests/required_output_parser_exprs.txt");
	REQUIRE(req == out);
}
