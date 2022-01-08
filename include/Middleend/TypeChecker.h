#pragma once
#include "Ast.h"
#include "ValueStorage.h"
#include "Scopes.h"

// TODO: Check that one doesn't do that: int* a = &1; <-- wrong.
// TODO: Also check that this: & &a; <-- cannot take the address of an address!
class TypeChecker : public IAstVisitor, public ValueStorage<Type>
{
public:
	TypeChecker(Scopes& scopes,NamespaceStmt& ns)
		:ValueStorage(this), scopes(scopes),ns(ns) {}
private:
	virtual void visit(DecimalLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(DelOpExpr& del) override;
	virtual void visit(FptrIdentExpr& fptr) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(FuncDeclStmt& func_decl) override;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
	virtual void visit(TypeDefStmt& coll_def) override;
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
	virtual void visit(MemOpExpr& mem) override;
	virtual void visit(SizeOrAlignmentInfoExpr& e) override;
	virtual void visit(OffsetofExpr& e) override;


	bool handle_bin_op_pointer_arithmetic(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_predefined(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_void(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_pointer_types(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_inferred(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_member_acc(BinOpExpr& bin_op);
	bool handle_bin_op_userdefined(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_copy(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_array(Type& tlh, Type& trh, BinOpExpr& bin_op);
	bool handle_bin_op_union(BinOpExpr& bin_op);
	void handle_ident_call_expr(FuncCallExpr& func_call_expr, IdentExpr* ident);
	std::pair<std::vector<Type>, bool> deduce_template_args(FuncDefStmt* def, const std::vector<Type>& params, const std::vector<uptr<TypeSpec>>& gparams, Expr& expr);

	FuncDefStmt* paste_new_function(FuncDefStmt* def, const std::vector<Type>& deduced_types, const Token& name, const std::vector<Type>& params, Expr& expr);

	void check_type_is_bool(uptr<Expr>& expr);
	std::pair<bool,uptr<Expr>> maybe_make_implicit_cast(const Type& from, const Type& to, uptr<Expr>&& child);

	Scopes& scopes;
	NamespaceStmt& ns;
	FuncDefStmt* current_function = nullptr;

	size_t new_elem_idx = 0;
};

void check_type(NamespaceStmt& ns, Scopes& sc);
void check_type_repeat(NamespaceStmt& ns, Scopes& sc);
