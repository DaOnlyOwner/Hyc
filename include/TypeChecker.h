#pragma once
#include "Ast.h"
#include "ValueStorage.h"
#include "Scopes.h"

class TypeChecker : public IAstVisitor, public ValueStorage<Type*>
{
public:
	TypeChecker(std::unique_ptr<Scopes> scopes)
		:ValueStorage(this), m_scopes(std::move(scopes)){}
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

	std::unique_ptr<Scopes> get_scopes()
	{
		return std::move(m_scopes);
	}

private:
	std::unique_ptr<Scopes> m_scopes;
	Type* m_type_to_pattern_match = nullptr;
	Type* m_current_func_ret_type = nullptr;
};