#include "Ast.h"

void IAstVisitor::visit(struct NamespaceStmt& namespace_stmt) { for (auto& p : namespace_stmt.stmts) p->accept(*this); };
void IAstVisitor::visit(struct IfStmt& if_stmt) {
	for (auto& p : if_stmt.if_stmts) p->accept(*this);
	for (auto& elif : if_stmt.all_elif_stmts)
	{
		for (auto& p : elif) p->accept(*this);
	}
	for (auto& p : if_stmt.else_stmts) p->accept(*this);
};

void IAstVisitor::visit(struct WhileStmt& while_stmt) {
	for (auto& p : while_stmt.stmts) p->accept(*this);
};

void IAstVisitor::visit(struct ForStmt& for_stmt) {
	for_stmt.decl_stmt->accept(*this);
	for (auto& p : for_stmt.stmts) p->accept(*this);
};

void IAstVisitor::visit(struct MatchStmt& match) {
	for (auto& case_ : match.match_cases)
	{
		case_.decl_stmt->accept(*this);
		for (auto& p : case_.body)
		{
			p->accept(*this);
		}
	}
};

void IAstVisitor::visit(struct ScopeStmt& sc) { for (auto& p : sc.stmts) p->accept(*this); };

void IAstVisitor::visit(struct ExprStmt& expr_stmt)
{
	expr_stmt.expr->accept(*this);
}

void IAstVisitor::visit(struct FuncDefStmt& func_def_stmt)
{
	func_def_stmt.decl->accept(*this);
	for (auto& p : func_def_stmt.body) p->accept(*this);
}
