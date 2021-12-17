#pragma once
#include "Ast.h"
#include "ValueStorage.h"
#include "Scopes.h"

// Returns if the expression is an lvalue and returns the name of the underlying identifier
class LValueChecker : public IAstVisitor, public ValueStorage<bool>
{
public:
	LValueChecker(Scopes& sc)
		:scopes(sc),ValueStorage(this) {}

private:
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

	Scopes& scopes;
};

void check_lvalues(Scopes& sc, NamespaceStmt& ns);
