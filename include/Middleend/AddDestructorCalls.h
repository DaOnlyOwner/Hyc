#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "Mangling.h"

class AddDestructorCalls : public IAstVisitor
{
public:
	AddDestructorCalls(Scopes& sc)
		:scopes(sc) {}

private:
	Scopes& scopes;
	void handle_descend(std::vector<uptr<Stmt>>& stmts);
	template<bool push_back>
	void handle_gen(size_t i, std::vector<uptr<Stmt>>& stmts)
	{
		auto vec = scopes.get_all_variables_reversed();
		for (auto& item : vec)
		{
			auto decl_stmt = item.second;
			if (decl_stmt->type.is_user_defined(scopes))
			{
				auto ie = std::make_unique<IdentExpr>(decl_stmt->name);
				ie->sem_type = decl_stmt->type;
				auto call = std::make_unique<DelOpExpr>(std::move(ie));
				call->sem_type = scopes.get_type("void");
				call->operation = scopes.get_op(mangle(*call));
				auto expr_stmt = std::make_unique<ExprStmt>(std::move(call));
				if constexpr (push_back)
					stmts.push_back(std::move(expr_stmt));
				else 
					stmts.insert(stmts.begin() + i, std::move(expr_stmt));
			}
		}
	}
	virtual void visit(TypeDefStmt& td)override {}
	virtual void visit(FuncDefStmt& f) override;
	virtual void visit(IfStmt& stmt) override;
	virtual void visit(WhileStmt& stmt) override;
	virtual void visit(ScopeStmt& scope) override;
	virtual void visit(MatchStmt& stmt) override;
};

void add_destructor_call(Scopes& sc, NamespaceStmt& ns);


