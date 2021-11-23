#include "ExpandScopes.h"

void ExpandScopes::visit(NamespaceStmt& namespace_stmt)
{
	scopes.expand();
	for (auto& stmt : namespace_stmt.stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(FuncDefStmt& func_call_stmt)
{
	scopes.expand();
	for (auto& stmt : func_call_stmt.body)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(StructDefStmt& struct_def_stmt)
{
	scopes.expand();
	for (auto& stmt : struct_def_stmt.stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(IfStmt& if_stmt)
{
	scopes.expand();
	for (auto& stmt : if_stmt.if_stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();

	
	for (auto& elif_stmt : if_stmt.all_elif_stmts)
	{
		scopes.expand();
		for (auto& stmt : elif_stmt)
		{
			stmt->accept(*this);
		}
		scopes.ascend();
	}

	scopes.expand();
	for (auto& else_stmt : if_stmt.elif_exprs)
	{
		else_stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(WhileStmt& while_stmt)
{
	scopes.expand();
	for (auto& stmt : while_stmt.stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(ForStmt& for_stmt)
{
	scopes.expand();
	for (auto& stmt : for_stmt.stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(UnionDefStmt& union_def)
{
	scopes.expand();
	for (auto& stmt : union_def.stmts)
	{
		stmt->accept(*this);
	}
	scopes.ascend();
}

void ExpandScopes::visit(MatchStmt& match)
{
	for (auto& case_ : match.match_cases)
	{
		scopes.expand();
		for (auto& stmt : case_.body)
		{
			stmt->accept(*this);
		}
		scopes.ascend();
	}
}
