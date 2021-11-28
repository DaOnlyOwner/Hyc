#pragma once
#include "Ast.h"
#include "Scopes.h"

// Creates types of the function arguments AND the return type
class CreateFuncArgsType : public IAstVisitor
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

void create_func_args_type(Scopes& sc, NamespaceStmt& ns);