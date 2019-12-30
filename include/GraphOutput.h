#pragma once
#include "Ast.h"
#include <string>

class GraphOutput : public IAstVisitor
{
public:
	void write_to_file(const std::string& filename);
private:
	// Geerbt über IAstVisitor
	virtual void visit(FloatLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(InferredDeclStmt& decl_inferred) override;
	virtual void visit(IdentPattern& ident) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& nms_stmt) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(FuncDefStmt& func_call_def_stmt) override;
	virtual void visit(ReturnStmt& ret_stmt) override;
	virtual void visit(ExprStmt& expr_stmt) override;
	std::string out = "digraph AST {";
	int node = 0;
	int new_name();
};
