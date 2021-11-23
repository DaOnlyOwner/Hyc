#pragma once
#include "Ast.h"
#include "Scopes.h"

class ExpandScopes : IAstVisitor
{
public:
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct FuncDefStmt& func_call_expr) override;
	virtual void visit(struct StructDefStmt& struct_def_stmt) override;
	virtual void visit(struct IfStmt& if_stmt) override;
	virtual void visit(struct WhileStmt& while_stmt) override;
	virtual void visit(struct ForStmt& for_stmt) override;
	virtual void visit(struct UnionDefStmt& union_def) override;
	virtual void visit(struct MatchStmt& match) override;
	Scopes scopes;
};