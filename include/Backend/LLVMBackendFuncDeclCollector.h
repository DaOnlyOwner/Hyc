#pragma once
#include "Ast.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

class LLVMBackendFuncDeclCollector : public IAstVisitor
{
public:
	LLVMBackendFuncDeclCollector(llvm::Module& mod, llvm::LLVMContext& context)
		:mod(mod),context(context) {}
	llvm::Module& mod;
	llvm::LLVMContext& context;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
};

void llvm_collect_funcs(NamespaceStmt& ns, llvm::Module& mod, llvm::LLVMContext& ctxt);
