#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"
#include <unordered_map>
#include "Mangling.h"
#include "LLVMBackendDefs.h"

// First bool is if a identifier should load the memory too
class LLVMBackendExpr : public IAstVisitor, public ValueStorageStack<llvm::Value*,bool>
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc)
		:scopes(sc), be(be), ValueStorageStack(this) {}

	llvm::Value* gen(Expr& expr, bool should_load = true)
	{
		return get_with_params(expr, should_load);
	}

	llvm::Value* create_assign_instr(const BinOpExpr& bin_op);
private:
	Scopes& scopes;
	LLVMBackendInfo& be;

	bool handle_pred(bool should_load, const BinOpExpr& bin_op);
	bool handle_member_acc(bool should_load, const BinOpExpr& bin_op);
	bool handle_union(const BinOpExpr& bin_op);
	llvm::Value* hack_ret_auto_gen(const BinOpExpr& bin_op, llvm::Value*& lhs);
	bool handle_assign(const BinOpExpr& bin_op);
	bool handle_ptr(const BinOpExpr& bin_op);

	llvm::Value* create_call_bin_op(const BinOpExpr& bin_op, llvm::Value* lhs, llvm::Value* rhs);
	llvm::CallInst* create_call_inst(FuncCallExpr& call, const std::variant<llvm::FunctionCallee, std::pair<llvm::FunctionType*, llvm::Value*>>& callee);

	llvm::Value* handle_sret_func_call(llvm::CallInst* callInst, FuncCallExpr& call);

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
	virtual void visit(FptrIdentExpr& fptr) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(ImplicitCastExpr& ice) override;
	virtual void visit(ArraySubscriptExpr& subs) override;
	virtual void visit(TernaryExpr& tern) override;
	virtual void visit(DelOpExpr& del) override;
	virtual void visit(MemOpExpr& mem) override;

};