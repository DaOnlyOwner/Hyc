#include "ElideReturnCopy.h"

void ElideReturnCopy::visit(BinOpExpr& bin)
{
	// Only called from ExprStmt.
	if (bin.op.type == Token::Specifier::Equal)
	{
		get_with_params(*bin.lh, nullptr);
		if (bin.rh->is_sret_func_call())
		{
			auto fcall = dynamic_cast<FuncCallExpr*>(bin.rh.get());
			get_with_params(*bin.rh, std::move(bin.lh));
			RETURN(std::move(bin.rh));
		}
	}
	RETURN(nullptr);
}

void ElideReturnCopy::visit(PrefixOpExpr& pre)
{
	get_with_params(*pre.lh, nullptr);
}

void ElideReturnCopy::visit(FuncCallExpr& fce)
{
	auto [arg] = std::move(get_params());
	get_with_params(*fce.from, nullptr);
	if (fce.def->decl->is_sret)
	{
		// Use the allocated value as parameter
		if (arg)
		{
			auto type = arg->sem_type;
			uptr<PrefixOpExpr> poe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Ampersand,"&"), std::move(arg));
			poe->sem_type = type.with_pointer();
			fce.arg_list.insert(fce.arg_list.begin(), { std::move(poe) });
		}
		// Allocate a new anonymous return value that will be passed as a parameter
		else
		{
			auto decl = std::make_unique<DeclStmt>(nullptr, Token(Token::Specifier::Ident,fmt::format("ret{}", ret_idx)));
			decl->type = fce.def->decl->arg_list[0]->type.with_pop();
			scopes.add(decl.get());
			ret_idx++;
			auto ie = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, decl->name.text));
			ie->sem_type = decl->type;
			auto poe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Ampersand, "&"), std::move(ie));
			poe->sem_type = decl->type.with_pointer();
			fce.arg_list.insert(fce.arg_list.begin(), { std::move(poe) });
			stmts->insert(stmts->begin(), std::move(decl));
		}
	}
}

void ElideReturnCopy::visit(PostfixOpExpr& post)
{
	get_with_params(*post.rh, nullptr);
}

void ElideReturnCopy::visit(ExprStmt& expr_stmt)
{
	auto& new_expr = get_with_params(*expr_stmt.expr, nullptr);
	if (new_expr)
	{
		expr_stmt.expr = std::move(new_expr);
	}
}

void ElideReturnCopy::visit(ReturnStmt& rt)
{
	if (!rt.returned_expr) return;
	auto& new_expr = get_with_params(*rt.returned_expr, nullptr);
	if (new_expr)
	{
		rt.returned_expr = std::move(new_expr);
	}
}

void ElideReturnCopy::visit(IfStmt& stmt)
{
	scopes.descend();
	stmts = &stmt.if_stmts;

	handle_descend(stmt.if_stmts);
	
	scopes.ascend();
	for (auto& p : stmt.all_elif_stmts)
	{
		scopes.descend();
		stmts = &p;
		handle_descend(p);
		scopes.ascend();
	}

	if (stmt.else_stmts.empty()) return;
	scopes.descend();
	stmts = &stmt.else_stmts;
	handle_descend(stmt.else_stmts);
	scopes.ascend();
}

void ElideReturnCopy::visit(WhileStmt& stmt)
{
	scopes.descend();
	stmts = &stmt.stmts;
	handle_descend(stmt.stmts);
	scopes.ascend();
}

void ElideReturnCopy::visit(ScopeStmt& stmt)
{
	scopes.descend();
	stmts = &stmt.stmts;
	handle_descend(stmt.stmts);
	scopes.ascend();
}

void ElideReturnCopy::visit(FuncDefStmt& stmt)
{
	scopes.descend();
	stmts = &stmt.body;
	handle_descend(stmt.body);
	scopes.ascend();
}

void ElideReturnCopy::handle_descend(std::vector<uptr<Stmt>>& stmts)
{
	size_t amount = stmts.size();
	for (int i = 0; i < amount; i++)
	{
		auto& p = stmts[i];
		p->accept(*this);
		size_t added_amount = stmts.size() - amount;
		amount = stmts.size();
		i += added_amount;
	}
}

void llvm_elide_return_copy(Scopes& sc, NamespaceStmt& ns)
{
	ElideReturnCopy erc(sc);
	ns.accept(erc);
}
