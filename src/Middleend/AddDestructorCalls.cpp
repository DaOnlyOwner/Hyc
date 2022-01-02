#include "AddDestructorCalls.h"

void AddDestructorCalls::handle_descend(std::vector<uptr<Stmt>>& stmts)
{
	size_t s = stmts.size();
	for (size_t i = 0; i < s; i++)
	{
		auto& p = stmts[i];
		p->accept(*this);
		if (p->is_terminator())
		{
			handle_gen<false>(i, stmts);
			return;
		}
	}
	handle_gen<true>(stmts.size(), stmts);
}

// TODO: Add continue and break to while loop
void AddDestructorCalls::visit(FuncDefStmt& f)
{
	scopes.descend();
	if (!f.decl->is_generated())
	{
		handle_descend(f.body);
	}
	scopes.ascend();
}

void AddDestructorCalls::visit(IfStmt& stmt)
{
	scopes.descend();
	handle_descend(stmt.if_stmts);
	scopes.ascend();

	for (auto& stmts : stmt.all_elif_stmts)
	{
		scopes.descend();
		handle_descend(stmts);
		scopes.ascend();
	}

	if (!stmt.else_stmts.empty())
	{
		scopes.descend();
		handle_descend(stmt.else_stmts);
		scopes.ascend();
	}
}

void AddDestructorCalls::visit(WhileStmt& stmt)
{
	scopes.descend();
	handle_descend(stmt.stmts);
	scopes.ascend();
}

void AddDestructorCalls::visit(ScopeStmt& scope)
{
	scopes.descend();
	handle_descend(scope.stmts);
	scopes.ascend();
}

void AddDestructorCalls::visit(MatchStmt& stmt)
{
	for (auto& case_ : stmt.match_cases)
	{
		scopes.descend();
		handle_descend(case_.body);
		scopes.ascend();
	}
}

void add_destructor_call(Scopes& sc, NamespaceStmt& ns)
{
	AddDestructorCalls adc(sc);
	ns.accept(adc);
}
