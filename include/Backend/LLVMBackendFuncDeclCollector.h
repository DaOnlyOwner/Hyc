#pragma once
#include "Ast.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "Scopes.h"

class LLVMBackendFuncDeclCollector : public IAstVisitor
{
public:
	LLVMBackendFuncDeclCollector(llvm::Module& mod, llvm::LLVMContext& context, Scopes& scopes)
		:mod(mod),context(context),scopes(scopes) {}
	llvm::Module& mod;
	llvm::LLVMContext& context;
	Scopes& scopes;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
};

void llvm_collect_funcs(NamespaceStmt& ns, llvm::Module& mod, llvm::LLVMContext& ctxt, Scopes& sc);
