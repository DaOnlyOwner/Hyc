#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "ValueStorage.h"

struct LLVMBackendInfo
{
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};

class LLVMBackendExpr : public IAstVisitor, public ValueStorage<llvm::Value*> 
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc)
		:scopes(sc),be(be), ValueStorage<llvm::Value*>(this) {}

	llvm::Value* gen(LLVMBackendInfo& be, Scopes& sc, NamespaceStmt& ns)
	{
		ns.accept(*this);
		return retrieve();
	}

private:
	Scopes& scopes;
	LLVMBackendInfo& be;
	bool handle_pred(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op);

	virtual void visit(DecimalLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(ExprStmt& expr_stmt) override;
	virtual void visit(ImplicitCastExpr& ice) override;
	virtual void visit(ArraySubscriptExpr& subs) override;
	virtual void visit(TernaryExpr& tern) override;

};
