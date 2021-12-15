#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "ValueStorage.h"
#include <unordered_map>

struct LLVMBackendInfo
{
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};

typedef std::unordered_map<std::string, std::unordered_map<std::string, llvm::Value*>> stack_allocated_mem;

std::string mangle(const FuncDeclStmt& decl);

class LLVMBackendExpr : public IAstVisitor, public ValueStorage<llvm::Value*> 
{
public:
	LLVMBackendExpr(LLVMBackendInfo& be, Scopes& sc,stack_allocated_mem& mem, const std::string& current_func_name)
		:scopes(sc),be(be), ValueStorage<llvm::Value*>(this),mem(mem),current_func_name(current_func_name) {}

	llvm::Value* gen(NamespaceStmt& ns)
	{
		ns.accept(*this);
		return retrieve();
	}

private:
	Scopes& scopes;
	LLVMBackendInfo& be;
	stack_allocated_mem& mem;
	const std::string& current_func_name;
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
