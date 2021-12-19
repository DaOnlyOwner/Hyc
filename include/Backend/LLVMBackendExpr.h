#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"
#include <unordered_map>
#include "Mangling.h"
#include "LLVMBackendDefs.h"

class LLVMBackendExpr : public IAstVisitor, public ValueStorage<llvm::Value*> 
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc,StackAllocatedMem& mem)
		:scopes(sc),be(be), ValueStorage<llvm::Value*>(this),mem(mem) {}

	llvm::Value* gen(Expr& expr, bool should_load = true)
	{
		this->should_load = should_load;
		this->is_member_start = true;
		expr.accept(*this);
		return retrieve();
	}

private:
	Scopes& scopes;
	LLVMBackendInfo& be;
	StackAllocatedMem& mem;
	
	bool handle_pred(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op);
	bool handle_member_acc(const BinOpExpr& bin_op);
	bool handle_assign(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op);

	llvm::Function* get_curr_fn()
	{
		return be.builder.GetInsertBlock()->getParent();
	}

	bool should_load = true;
	bool is_member_start = true;

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
