#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "CodePaster.h"

class GenericInst : IAstVisitor
{
public:
	GenericInst(Scopes& scopes)
		:scopes(scopes){}


private:
	virtual void visit(NamespaceStmt& stmt) override;
	virtual void visit(FuncDeclStmt& stmt) override;
	virtual void visit(DeclStmt& stmt) override;
	virtual void visit(StructDefStmt& stmt) override;
	virtual void visit(UnionDefStmt& stmt) override;
	virtual void visit(BaseTypeSpec& bt_spec) override;

	Scopes& scopes;
	NamespaceStmt* top_level = nullptr;
};

void instantiate_generic(const BaseTypeSpec& bt, Scopes& scopes);
