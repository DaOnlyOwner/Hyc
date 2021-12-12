#include "ExpandScopes.h"

void ExpandScopes::visit(NamespaceStmt& namespace_stmt)
{
	if(entry == 0) scopes.expand();
	for (int i = entry; i<namespace_stmt.stmts.size(); i++)
	{
		auto& stmt = namespace_stmt.stmts[i];
		stmt->accept(*this);
	}
	if(entry == 0) scopes.ascend();
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

void ExpandScopes::visit(CollectionStmt& coll_def)
{
	scopes.expand();
	for (auto& stmt : coll_def.stmts)
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

void ExpandScopes::visit(ScopeStmt& sc)
{
	scopes.descend();
	for (auto& p : sc.stmts) p->accept(*this);
	scopes.ascend();
}

void expand_scopes(NamespaceStmt& ns, Scopes& sc, size_t entry)
{
	ExpandScopes es(sc,entry);
	ns.accept(es);
}