#include "LLVMBackendFuncDeclCollector.h"
#include "Mangling.h"
#include "TypeToLLVMType.h"
#include <algorithm>
#include "llvm/IR/Function.h"

void LLVMBackendFuncDeclCollector::visit(FuncDefStmt& func_def_stmt)
{
	std::vector<llvm::Type*> args;
	std::transform(func_def_stmt.decl->arg_list.begin(), func_def_stmt.decl->arg_list.end(), std::back_inserter(args), [&](const uptr<DeclStmt>& ds) {
		return map_type(ds->type,scopes,context);
		});
	auto res_t = map_type(func_def_stmt.decl->ret_type->semantic_type,scopes,context);
	auto ft = llvm::FunctionType::get(res_t, args, false);
	auto func = llvm::Function::Create(ft,llvm::Function::ExternalLinkage, mangle(*func_def_stmt.decl),mod);
	int idx = 0;
	for (auto& arg : func->args())
	{
		arg.setName(func_def_stmt.decl->arg_list[idx]->name.text);
		idx++;
	}
}

void llvm_collect_funcs(NamespaceStmt& ns,llvm::Module& mod, llvm::LLVMContext& ctxt, Scopes& sc)
{
	LLVMBackendFuncDeclCollector fdc(mod, ctxt,sc);
	ns.accept(fdc);
}