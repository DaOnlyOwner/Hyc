#include "Desugar.h"

void DesugarAccess::visit(BinOpExpr& bin)
{
	bin.lh->accept(*this);
	bin.rh->accept(*this);
}

void DesugarAccess::visit(PrefixOpExpr& pre)
{
	pre.lh->accept(*this);
}


void DesugarAccess::visit(PostfixOpExpr& post)
{
	post.rh->accept(*this);
}

void DesugarAccess::visit(FuncCallExpr& expr)
{
	auto* bin_op = dynamic_cast<BinOpExpr*>(expr.from.get());
	if (!bin_op)
	{
		expr.from->accept(*this);
		return;
	}

	if (bin_op->op.type == Token::Specifier::MemAccess || bin_op->op.type == Token::Specifier::Dot)
	{
		auto ie = std::make_unique<IdentExpr>(std::move(bin_op->rh));
		uptr<Expr> transformed;
		bin_op->lh->accept(*this);
		if (bin_op->op.type == Token::Specifier::MemAccess)
			transformed = std::move(bin_op->lh);
		else
		{
			Token tk = bin_op->op;
			tk.text = "&";
			tk.type = Token::Specifier::Ampersand;
			transformed = std::make_unique<PrefixOpExpr>(tk, std::move(bin_op->lh));
		}
		FuncCallArg fca;
		fca.expr = std::move(transformed);
		fca.moved = false;
		expr.arg_list.insert(expr.arg_list.begin(), std::move(fca));
	}

	for (auto& p : expr.arg_list) p.expr->accept(*this);
}

void DesugarForStmt::visit(NamespaceStmt& ns)
{
	update(ns.stmts);
}

void DesugarForStmt::visit(IfStmt& if_stmt)
{
	update(if_stmt.if_stmts);
	for (auto& elif : if_stmt.all_elif_stmts) update(elif);
	update(if_stmt.else_stmts);
}

void DesugarForStmt::visit(WhileStmt& while_stmt)
{
	update(while_stmt.stmts);
}

void DesugarForStmt::visit(MatchStmt& match)
{
	for (auto& cases : match.match_cases) update(cases.body);
}

void DesugarForStmt::visit(ScopeStmt& sc)
{
	update(sc.stmts);
}

void DesugarForStmt::update(std::vector<uptr<Stmt>>& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		auto& p = vec[i];
		ret(nullptr);
		auto& maybe_while = get(p);
		if (maybe_while != nullptr)
		{
			vec[i] = std::move(maybe_while);
		}
	}
}

void DesugarForStmt::visit(FuncDefStmt& fds)
{
	update(fds.body);
}

void DesugarForStmt::visit(ForStmt& for_stmt)
{
	std::vector<std::unique_ptr<Stmt>> scope;
	scope.push_back(std::move(for_stmt.decl_stmt));
	auto body = mv(for_stmt.stmts);
	body.push_back(std::make_unique<ExprStmt>(mv(for_stmt.snd_expr)));
	uptr<WhileStmt> ws = std::make_unique<WhileStmt>(std::move(for_stmt.fst_expr), mv(body));
	RETURN(std::move(ws));
}

void DesugarDecl::visit(IfStmt& if_stmt)
{
	update(if_stmt.if_stmts);
	for (auto& elif : if_stmt.all_elif_stmts) update(elif);
	update(if_stmt.else_stmts);
}

void DesugarDecl::visit(WhileStmt& while_stmt)
{
	update(while_stmt.stmts);
}

void DesugarDecl::visit(MatchStmt& match)
{
	for (auto& cases : match.match_cases)
	{
		update(cases.body);
	}
}

void DesugarDecl::visit(ScopeStmt& sc)
{
	update(sc.stmts);
}

void DesugarDecl::visit(FuncDefStmt& func_def_stmt)
{
	update(func_def_stmt.body);
}

void DesugarDecl::visit(DeclCpyStmt& cpy)
{
	make_new_stmts(cpy,Token::Specifier::Equal);
}

void DesugarDecl::visit(DeclMvStmt& mv)
{
	make_new_stmts(mv,Token::Specifier::Hashtag);
}

void DesugarDecl::visit(DeclInitStmt& init)
{
	uptr<DeclStmt> decl_new = std::make_unique<DeclStmt>(mv(init.type), std::move(init.name));
	uptr<ExprStmt> sexpr = std::make_unique<ExprStmt>(mv(init.expr));
	auto& [decl, expr] = retrieve();
	
}

void DesugarDecl::update(std::vector<uptr<Stmt>>& to_update)
{
	for (size_t i = 0; i < to_update.size(); i++)
	{
		ret(std::make_pair(nullptr,nullptr));
		auto& p = get(to_update[i]);
		to_update[i] = std::move(p.first);
		to_update.insert(to_update.begin() + i+1, std::move(p.second));
	}
}

void desugar(NamespaceStmt& ns)
{
	DesugarForStmt dfs;
	ns.accept(dfs);

	DesugarAccess da;
	ns.accept(da);

	DesugarDecl dd;
	ns.accept(dd);
}
