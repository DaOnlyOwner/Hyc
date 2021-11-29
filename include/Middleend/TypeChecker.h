#pragma once
#include "Ast.h"
#include "ValueStorage.h"
#include "Scopes.h"

class TypeChecker : public IAstVisitor, public ValueStorage<Type>
{
public:
	TypeChecker(Scopes& scopes,NamespaceStmt& ns)
		:ValueStorage(this), scopes(scopes),ns(ns) {}
private:
	virtual void visit(struct FloatLiteralExpr& lit) override;
	virtual void visit(struct IntegerLiteralExpr& lit) override;
	virtual void visit(struct BinOpExpr& bin_op) override;
	virtual void visit(struct PrefixOpExpr& pre_op) override;
	virtual void visit(struct PostfixOpExpr& post_op) override;
	virtual void visit(struct IdentExpr& ident) override;
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct FuncCallExpr& func_call_expr) override;
	virtual void visit(struct FuncDeclStmt& func_decl) override;
	virtual void visit(struct FuncDefStmt& func_def_stmt) override;
	virtual void visit(struct CollectionStmt& coll_def) override;
	virtual void visit(struct ReturnStmt& ret_stmt) override;
	virtual void visit(struct ExprStmt& expr_stmt) override;
	virtual void visit(struct DeclStmt& decl_stmt) override;
	virtual void visit(struct PointerTypeSpec& pt_spec) override;
	virtual void visit(struct BaseTypeSpec& bt_spec) override;
	virtual void visit(struct ArrayTypeSpec& at_spec) override;
	virtual void visit(struct FptrTypeSpec& fptr) override;
	virtual void visit(struct ScopeTypeSpec& scope_spec) override;
	virtual void visit(struct ImplicitCastExpr& ice) override;
	virtual void visit(struct IfStmt& if_stmt) override;
	virtual void visit(struct WhileStmt& while_stmt) override;
	virtual void visit(struct ContinueStmt& cont_stmt) override;
	virtual void visit(struct ArraySubscriptExpr& subs) override;
	virtual void visit(struct TernaryExpr& tern) override;
	virtual void visit(struct MatchStmt& match) override;
	virtual void visit(struct ScopeStmt& sc) override;

	Scopes& scopes;
	NamespaceStmt& ns;
};

//
//	std::unique_ptr<Scopes> get_scopes()
//	{
//		return std::move(m_scopes);
//	}
//
//private:
//	std::unique_ptr<Scopes> m_scopes;
//	FuncDefStmt* m_current_func = nullptr;
//	bool return_stmt_in_func = false;
//	// Geerbt über IAstVisitor
//	virtual void visit(FloatLiteralExpr& lit) override;
//	virtual void visit(IntegerLiteralExpr& lit) override;
//	virtual void visit(BinOpExpr& bin_op) override;
//	virtual void visit(PrefixOpExpr& pre_op) override;
//	virtual void visit(PostfixOpExpr& post_op) override;
//	virtual void visit(InferredDefStmt& decl_inferred) override;
//	virtual void visit(IdentExpr& ident) override;
//	virtual void visit(NamespaceStmt& namespace_stmt) override;
//	virtual void visit(FuncCallExpr& func_call_expr) override;
//	virtual void visit(FuncDefStmt& func_call_def_stmt) override;
//	virtual void visit(ReturnStmt& ret_stmt) override;
//	virtual void visit(ExprStmt& expr_stmt) override;
//	virtual void visit(DefStmt& def_stmt) override;
//	virtual void visit(PointerTypeSpec& pt_spec) override;
//	virtual void visit(BaseTypeSpec& bt_spec) override;
//	virtual void visit(ArrayTypeSpec& at_spec) override;
//	virtual void visit(ImplicitCastExpr& ice) override;
//
//};