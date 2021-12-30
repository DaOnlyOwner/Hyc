#include "PromoteToSret.h"

void PromoteToSret::visit(FuncDefStmt& func_def)
{
	if (!(func_def.decl->ret_type->semantic_type.is_predefined(scopes)
		|| func_def.decl->ret_type->semantic_type.is_pointer_type()))
	{
		func_def.decl->is_sret = true;
		auto ret_t = func_def.decl->ret_type->semantic_type;
		func_def.decl->ret_type->semantic_type = scopes.get_type("void");
		ret_t.promote_pointer();
		uptr<DeclStmt> decl = std::make_unique<DeclStmt>(nullptr, Token(Token::Specifier::Ident, ".ret"));
		decl->type = ret_t;
		decl->is_sret = true;
		auto ident = std::make_unique<IdentExpr>(decl->name);
		ident->sem_type = decl->type;
		ident->decl = decl.get();
		expr = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"),std::move(ident));
		expr->sem_type = decl->type.with_pop();
		func_def.decl->arg_list.insert(func_def.decl->arg_list.begin(), std::move(decl));
		
		make_new_stmt(func_def.body);
	}
}

void PromoteToSret::visit(ReturnStmt& rs)
{
	// This is the place for a move 
	auto clone = expr->clone();
	auto bin = std::make_unique<BinOpExpr>(Token(Token::Specifier::Equal, "="), std::move(clone), std::move(rs.returned_expr));
	bin->sem_type = scopes.get_type("void");
	auto stmt = std::make_unique<ExprStmt>(std::move(bin));
	auto ret_void = std::make_unique<ReturnStmt>(nullptr, Token());
	ret({ std::move(stmt),std::move(ret_void) });
}

void PromoteToSret::visit(IfStmt& if_stmt)
{
	make_new_stmt(if_stmt.if_stmts);
	for (auto& e : if_stmt.all_elif_stmts)
	{
		make_new_stmt(e);
	}
	make_new_stmt(if_stmt.else_stmts);
}

void PromoteToSret::visit(WhileStmt& while_stmt)
{
	make_new_stmt(while_stmt.stmts);
}

void PromoteToSret::visit(ScopeStmt& scope)
{
	make_new_stmt(scope.stmts);
}

void PromoteToSret::make_new_stmt(std::vector<uptr<Stmt>>& stmts)
{
	for (int i = 0; i < stmts.size(); i++)
	{
		auto& p = stmts[i];
		auto& [fst, snd] = get(p);
		if (p->is_return_stmt())
		{
			stmts[i] = std::move(fst);
			stmts.insert(stmts.begin() + i+1, std::move(snd));
			return;
		}
	}
}

void llvm_promote_to_sret(Scopes& sc, NamespaceStmt& ns)
{
	PromoteToSret pts(sc);
	ns.accept(pts);
}
