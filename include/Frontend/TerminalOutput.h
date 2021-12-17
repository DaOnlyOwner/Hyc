#pragma once
#include "Ast.h"

class TerminalOutput : public IAstVisitor
{
public:
	const std::string& get_format_str() const { return out; }
	std::string& get_format_str() { return out; }
private:
	// Geerbt über IAstVisitor
	virtual void visit(DecimalLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(DeclCpyStmt& decl_cpy) override;
	virtual void visit(DeclMvStmt& decl_mv) override;
	virtual void visit(DeclInitStmt& decl_init) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& namespace_stmt) override;
	virtual void visit(FuncCallExpr& func_call_expr) override;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
	virtual void visit(ReturnStmt& ret_stmt) override;
	virtual void visit(ExprStmt& expr_stmt) override;
	virtual void visit(DeclStmt& decl_stmt) override;
	virtual void visit(PointerTypeSpec& pt_spec) override;
	virtual void visit(BaseTypeSpec& bt_spec) override;
	virtual void visit(ArrayTypeSpec& at_spec) override;
	virtual void visit(ImplicitCastExpr& ice) override;
	virtual void visit(IfStmt& if_stmt) override;
	virtual void visit(WhileStmt& while_stmt) override;
	virtual void visit(ForStmt& for_stmt) override;
	virtual void visit(CollectionStmt& collection_def) override;
	virtual void visit(ContinueStmt& cont_stmt) override;
	virtual void visit(FptrTypeSpec& fptr) override;
	virtual void visit(ArraySubscriptExpr& subs) override;
	virtual void visit(TernaryExpr& tern) override;
	virtual void visit(FuncDeclStmt& func_decl) override;
	virtual void visit(MatchStmt& match) override;
	virtual void visit(ScopeStmt& sc) override;

	int indent = 0;
	void make_indent();
	std::string out = "";








};


