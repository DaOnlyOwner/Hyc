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
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};

typedef std::unordered_map<std::string, std::unordered_map<std::string, llvm::Value*>> stack_allocated_mem;

class LLVMBackendExpr : public IAstVisitor, public ValueStorage<llvm::Value*> 
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc,stack_allocated_mem& mem)
		:scopes(sc),be(be), ValueStorage<llvm::Value*>(this),mem(mem) {}

	llvm::Value* gen(Expr& expr)
	{
		expr.accept(*this);
		return retrieve();
	}

	void set_curr_func_name(std::string& name)
	{
		current_func_name = &name;
	}


private:
	Scopes& scopes;
	LLVMBackendInfo& be;
	stack_allocated_mem& mem;
	std::string* current_func_name=nullptr;
	bool handle_pred(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op);

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
