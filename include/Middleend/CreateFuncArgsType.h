#pragma once
#include "Ast.h"
#include "Scopes.h"

// Creates types of the function arguments AND the return type
class CreateFuncArgsType : IAstVisitor
{
public:
	CreateFuncArgsType(Scopes& sc,NamespaceStmt& ns)
		:scopes(sc),ns(ns){}

private:
	virtual void visit(NamespaceStmt& ns);
	virtual void visit(FuncDefStmt& def);

	Scopes& scopes;
	NamespaceStmt& ns;
};