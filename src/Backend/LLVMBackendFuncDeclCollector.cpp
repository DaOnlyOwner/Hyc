#include "LLVMBackendFuncDeclCollector.h"
#include "Mangling.h"
#include "TypeToLLVMType.h"
#include <algorithm>
#include "llvm/IR/Function.h"

void LLVMBackendFuncDeclCollector::visit(FuncDefStmt& func_def_stmt)
{
	std::vector<llvm::Type*> args;
	std::transform(func_def_stmt.decl->arg_list.begin(), func_def_stmt.decl->arg_list.end(), std::back_inserter(args), [](const DeclStmt& ds) {
		return map_type(ds.type);
		});
	auto res_t = map_type(func_def_stmt.decl->ret_type->semantic_type);
	auto ft = llvm::FunctionType::get(res_t, args, false);
	llvm::Function::Create(ft,llvm::Function::InternalLinkage, mangle(*func_def_stmt.decl),mod);
}

void llvm_collect_funcs(NamespaceStmt& ns,llvm::Module& mod, llvm::LLVMContext& ctxt)
{
	LLVMBackendFuncDeclCollector fdc(mod, ctxt);
	ns.accept(fdc);
}
