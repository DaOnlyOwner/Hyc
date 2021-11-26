#pragma once

#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"
#include "AtomicType.h"

class DeclarationsCollectorFunctions : public IAstVisitor
{
public:
	DeclarationsCollectorFunctions(Scopes& scopes,NamespaceStmt& ns)
		:scopes(scopes),ns(ns) {};

private:
	Scopes& scopes;
	NamespaceStmt& ns;

	virtual void visit(NamespaceStmt& ns) override;
	virtual void visit(FuncDefStmt& func_def_stmt) override;
};