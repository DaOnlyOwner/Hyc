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
#include "LLVMBackendFuncDeclCollector.h"
#include "fmt/color.h"
#include "TerminalOutput.h"
#include <fstream>
#include "ArrayDeclChecker.h"
#include "TaggedValueChecker.h"
#include "LLVMBackendFuncDeclCollector.h"
#include "LLVMBackendMemberCollector.h"
#include "LLVMBackendTypeCollector.h"
#include "PromoteToSret.h"
#include "ElideReturnCopy.h"
#include "CircularEmbedChecker.h"
#include "GenerateOperators.h"
#include "OperatorGenerationFuncs.h"
#include "AddDestructorCalls.h"

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
		return parsed;
	}
}

int Pipeline::build(const std::string& filename, const LLVMBackend::CompilerInfo& ci)
{
	Scopes sc;
	Tree<TypeDefStmt*> type_hierachy;

	std::function<gen_func_t> gen_destr_struct = op_gen_funcs::gen_destructor_struct;
	std::function<gen_func_t> gen_destr_union = op_gen_funcs::gen_destructor_union;
	std::function<gen_arglist_t> gen_destr_arg = op_gen_funcs::gen_destructor_arglist;

	std::function<gen_func_t> gen_copy_struct = op_gen_funcs::gen_copy_struct;
	std::function<gen_func_t> gen_move_struct = op_gen_funcs::gen_move_struct;

	std::function<gen_func_t> gen_copy_move_union = op_gen_funcs::gen_copy_move_union;
	std::function<gen_arglist_t> gen_copy_move_arg = op_gen_funcs::gen_copy_move_arglist;



	auto parsed = parse(filename);
	// Just used for debug
	if (!parsed) return 1;
	desugar(*parsed);
	collect_types(*parsed, sc, type_hierachy);
	bool error = check_default_type_arg(*parsed, sc);
	if (error) { return 1; }
	create_func_args_type(sc, *parsed);
	instantiate_generic_repeat(*parsed, sc);
	collect_funcs(*parsed, sc);
	collect_members(*parsed, sc);
	check_tagged_values(*parsed, 0);
	gen_op(sc, *parsed, gen_destr_struct, gen_destr_union, gen_destr_arg,"del");
	gen_op(sc, *parsed, gen_copy_struct, gen_copy_move_union, gen_copy_move_arg, "=");
	gen_op(sc, *parsed, gen_move_struct, gen_copy_move_union, gen_copy_move_arg, "#");

	check_type_repeat(*parsed, sc);
	check_lvalues(sc, *parsed);
	check_circular_embed(*parsed, type_hierachy, sc);
	if (ci.emit_info == LLVMBackend::CompilerInfo::EmitInfo::EmitAST)
	{
		add_destructor_call(sc, *parsed);
		TerminalOutput to;
		parsed->accept(to);
		if(ci.emit_to_stdout)
			fmt::print("{}", to.get_format_str());
		else
		{
			std::ofstream f;
			f.open(ci.filename_output);
			if (!f)
			{
				fmt::print(fmt::fg(fmt::color::orange_red), "Cannot open file '{}' for writing", ci.filename_output);
			}
			f << to.get_format_str();
		}
		return 0;
	}
	if (Error::Error)
	{
		return 1;
	}
	LLVMBackend backend(*parsed, sc);
	auto& be = backend.init();
	// Preparation passes
	llvm_collect_types(be, *parsed);
	llvm_collect_member(be, sc, *parsed);
	llvm_promote_to_sret(sc, *parsed);
	llvm_elide_return_copy(sc, *parsed);
	add_destructor_call(sc, *parsed);
	llvm_collect_funcs(*parsed, be.mod, be.context, sc);
	// End prep passes
	if (Error::Error) return 1;
	return backend.emit(ci,filename);
}
