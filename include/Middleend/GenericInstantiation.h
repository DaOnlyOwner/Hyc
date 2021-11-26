#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "CodePaster.h"

class GenericInst : IAstVisitor
{
public:
	GenericInst(Scopes& scopes, NamespaceStmt& stmt)
		:scopes(scopes),top_level(stmt) {}

	void instantiate(BaseTypeSpec& bt)
	{
		bt.accept(*this);
	}

private:
	virtual void visit(NamespaceStmt& stmt) override;
	virtual void visit(FuncDefStmt& stmt) override;
	virtual void visit(DeclStmt& stmt) override;
	virtual void visit(StructDefStmt& stmt) override;
	virtual void visit(UnionDefStmt& stmt) override;
	virtual void visit(BaseTypeSpec& bt_spec) override;

	Scopes& scopes;
	NamespaceStmt& top_level;
};

void instantiate_generic(BaseTypeSpec& bt, Scopes& scopes, NamespaceStmt& ns);
