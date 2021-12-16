#include "Pipeline.h"
#include "Parser.h"
#include "TypeChecker.h"
#include "ExpandScopes.h"
#include "DeclarationsCollectorTypes.h"
#include "DeclarationsCollectorFunctions.h"
#include "CreateFuncArgsType.h"
#include "GenericInstantiation.h"
#include "DefaultTypeArgChecker.h"
#include "Desugar.h"
#include "MemberCollector.h"
#include "TypeChecker.h"
#include "LValueChecker.h"
#include "LLVMBackend.h"
#include "fmt/color.h"

namespace
{
	uptr<NamespaceStmt> parse(const std::string& filename)
	{
		std::ifstream fileInput(filename);
		if (!fileInput)
		{
			fmt::print(fmt::fg(fmt::color::dark_red), "File '{}' not found", filename);
			return nullptr;
		}
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
			return nullptr;
		}


	}
}

int Pipeline::build(std::ifstream& input, const std::string& filename, const LLVMBackend::CompilerInfo& ci)
{
	Scopes sc;
	auto parsed = parse(filename);
	if (!parsed) return 1;
	desugar(*parsed);
	collect_types(*parsed, sc);
	bool error = check_default_type_arg(*parsed, sc);
	if (error) { return 1; }
	create_func_args_type(sc, *parsed);
	instantiate_generic_repeat(*parsed, sc);
	collect_funcs(*parsed, sc);
	collect_members(*parsed, sc);
	int n = parsed->stmts.size();
	check_type_repeat(*parsed, sc);
	check_lvalues(sc, *parsed);
	LLVMBackend backend(*parsed, sc);
	return backend.emit(ci,filename);
}
