#pragma once
#include "Ast.h"
#include "LLVMBackendExpr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "ValueStorage.h"

class LLVMBackendStmt : public IAstVisitor, public ValueStorage<llvm::AllocaInst*>
{
public:
	LLVMBackendStmt(LLVMBackendInfo& be,Scopes& sc)
		:be(be),scopes(sc),expr_getter(be,sc,mem),ValueStorage(this) {}
private:

	void insert_bb(llvm::BasicBlock* bb)
	{
		auto current_function = be.builder.GetInsertBlock()->getParent();
		//current_function->getBasicBlockList().push_back(bb);
		be.builder.SetInsertPoint(bb);
	}

	llvm::Function* get_curr_fn()
	{
		return be.builder.GetInsertBlock()->getParent();
	}

	// This is from the kaleidoskop tutorial
	llvm::AllocaInst* create_alloca(const Type& t, const std::string& name);

	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(DeclStmt& decl_stmt) override;
	virtual void visit(FuncDefStmt& func_def) override;
	virtual void visit(ReturnStmt& return_stmt) override;
	virtual void visit(FuncDeclStmt& func_decl) override;

	StackAllocatedMem mem;
	LLVMBackendInfo& be;
	Scopes& scopes;
	LLVMBackendExpr expr_getter;
};