#if 0
#pragma once
#include "Ast.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "ValueStorage.h"

class CodeGen : public IAstVisitor, ValueStorage<llvm::Value*>
{
	CodeGen();
	// Geerbt über IAstVisitor
	virtual void visit(FloatLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(InferredDeclStmt& decl_inferred) override;
	virtual void visit(IdentPattern& ident) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(FuncDefStmt& func_call_def_stmt) override;
	virtual void visit(ReturnStmt& ret_stmt) override;
	virtual void visit(ExprStmt& expr_stmt) override;

private:
	llvm::LLVMContext m_context;
	llvm::IRBuilder<> m_ir_builder;
	llvm::Module m_module;
};
#endif