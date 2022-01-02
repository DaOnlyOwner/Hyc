#include "LLVMBackendFuncDeclCollector.h"
#include "Mangling.h"
#include "TypeToLLVMType.h"
#include <algorithm>
#include "llvm/IR/Function.h"

void LLVMBackendFuncDeclCollector::visit(FuncDefStmt& func_def_stmt)
{
	if (!func_def_stmt.decl->generic_list.empty()) return;
	std::vector<llvm::Type*> args;
	std::transform(func_def_stmt.decl->arg_list.begin(), func_def_stmt.decl->arg_list.end(), std::back_inserter(args), [&](const uptr<DeclStmt>& ds) {
		return map_type(ds->type,scopes,context);
		});
	auto res_t = map_type(func_def_stmt.decl->ret_type->semantic_type,scopes,context);
	auto ft = llvm::FunctionType::get(res_t, args, false);
	auto func = llvm::Function::Create(ft,
		func_def_stmt.decl->name.text == "main" ? llvm::Function::ExternalLinkage : llvm::Function::InternalLinkage,
		mangle(func_def_stmt), mod);
	int idx = 0;
	for (auto& arg : func->args())
	{
		arg.setName(func_def_stmt.decl->arg_list[idx]->name.text);
		idx++;
	}
	if (func_def_stmt.decl->is_sret)
	{
		llvm::AttrBuilder ab;
		ab.addStructRetAttr(map_type(func_def_stmt.decl->arg_list[0]->type.with_pop(), scopes, context));
		ab.addAttribute(llvm::Attribute::get(context, llvm::Attribute::AttrKind::NoAlias));
		func->args().begin()->addAttrs(ab);
	}
}

void llvm_collect_funcs(NamespaceStmt& ns,llvm::Module& mod, llvm::LLVMContext& ctxt, Scopes& sc)
{
	LLVMBackendFuncDeclCollector fdc(mod, ctxt,sc);
	ns.accept(fdc);
}
