//#pragma once
//#include "Ast.h"
//#include "ValueStorage.h"
//#include "Scopes.h"
//
//class TypeChecker : public IAstVisitor, public ValueStorage<Type>
//{
//public:
//	TypeChecker(std::unique_ptr<Scopes> scopes)
//		:ValueStorage(this), m_scopes(std::move(scopes)){}
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