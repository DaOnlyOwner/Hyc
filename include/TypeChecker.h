#pragma once
#include "Ast.h"
#include "ValueStorage.h"
#include "UID.h"
#include "Scopes.h"

class TypeChecker : public AstVisitor, public ValueStorage<UID>
{
	// Geerbt �ber AstVisitor
	virtual void visit(FloatLiteralExpr& lit) override;
	virtual void visit(IntegerLiteralExpr& lit) override;
	virtual void visit(BinOpExpr& bin_op) override;
	virtual void visit(PrefixOpExpr& pre_op) override;
	virtual void visit(PostfixOpExpr& post_op) override;
	virtual void visit(InferredDeclStmt& decl_inferred) override;
	virtual void visit(IdentPattern& ident) override;
	virtual void visit(IdentExpr& ident) override;
	virtual void visit(NamespaceStmt& namespace_stmt) override;
private:
	Scopes m_scopes;
};