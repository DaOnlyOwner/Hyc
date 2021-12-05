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
	virtual void visit(FloatLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(FuncDeclStmt& func_decl) override;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
	virtual void visit(CollectionStmt& coll_def) override;
	virtual void visit(ReturnStmt& ret_stmt) override;
	virtual void visit(ExprStmt& expr_stmt) override;
	virtual void visit(DeclStmt& decl_stmt) override;
	virtual void visit(ImplicitCastExpr& ice) override;
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(ContinueStmt& cont_stmt) override;
	virtual void visit(ArraySubscriptExpr& subs) override;
	virtual void visit(TernaryExpr& tern) override;
	virtual void visit(MatchStmt& match) override;
	virtual void visit(ScopeStmt& sc) override;

	bool handle_bin_op_pointer_arithmetic(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_predefined(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_void(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_pointer_types(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_inferred(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_overloads(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_member_acc(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_copy_move(Type& tlh, Type& trh, BinOpExpr& bin_op);


	Scopes& scopes;
	NamespaceStmt& ns;
};

void check_type(NamespaceStmt& ns, Scopes& sc);

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