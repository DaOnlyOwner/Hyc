#pragma once
#include "Ast.h"
#include "LLVMBackendExpr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "ValueStorage.h"

class LLVMBackendStmt : public IAstVisitor
{
public:
	LLVMBackendStmt(LLVMBackendInfo& be,Scopes& sc)
		:be(be),scopes(sc),expr_getter(be,sc,mem) {}
private:
	virtual void visit(FuncDefStmt& func_decl) override;
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	llvm::Function* current_function = nullptr;
	FuncDefStmt* current_func_def = nullptr;
	stack_allocated_mem mem;
	LLVMBackendInfo& be;
	Scopes& scopes;
	LLVMBackendExpr expr_getter;
};