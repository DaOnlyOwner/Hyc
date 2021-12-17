#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "ValueStorage.h"
#include <unordered_map>
#include "Mangling.h"

struct LLVMBackendInfo
{
	LLVMBackendInfo()
		:context(),mod("global",context),builder(context){}
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};

struct StackAllocatedMem
{
	std::unordered_map<std::string, llvm::Value*> global_mem;
	std::unordered_map<std::string, llvm::Value*> function_stack;

	void enter_function(llvm::Function* func)
	{
		function_stack.clear();
		for (auto& arg : func->args())
		{
			function_stack[arg.getName().str()] = &arg;
		}
	}
};

class LLVMBackendExpr : public IAstVisitor, public ValueStorage<llvm::Value*> 
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc,StackAllocatedMem& mem)
		:scopes(sc),be(be), ValueStorage<llvm::Value*>(this),mem(mem) {}

	llvm::Value* gen(Expr& expr)
	{
		expr.accept(*this);
		return retrieve();
	}

private:
	Scopes& scopes;
	LLVMBackendInfo& be;
	StackAllocatedMem& mem;
	
	bool handle_pred(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op);
	llvm::Function* get_curr_fn()
	{
		return be.builder.GetInsertBlock()->getParent();
	}

	virtual void visit(DecimalLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(ImplicitCastExpr& ice) override;
	virtual void visit(ArraySubscriptExpr& subs) override;
	virtual void visit(TernaryExpr& tern) override;
};