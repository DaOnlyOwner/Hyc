#pragma once
#include "Ast.h"
#include "Scopes.h"

class DeclarationsCollectorTypes : IAstVisitor
{
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct StructDefStmt& struct_def_stmt) override;
	virtual void visit(struct UnionDefStmt& union_def) override;
	Scopes scopes;
};