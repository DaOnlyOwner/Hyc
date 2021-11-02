#include "DeclarationsCollector.h"
#include <algorithm>
#include "DebugPrint.h"
#include <iterator>

DeclarationsCollector::DeclarationsCollector()
{
	m_scopes = std::make_unique<Scopes>();
}

void DeclarationsCollector::visit(NamespaceStmt& namespace_stmt)
{
	m_scopes->expand();
	for (auto& stmt : namespace_stmt.stmts)
	{
		stmt->accept(*this);
	}
	m_scopes->ascend();
}

void DeclarationsCollector::visit(FuncDefStmt& func_def_stmt)
{
	std::vector<Type*> args_semantic;
	std::transform(func_def_stmt.arg_list_type_ident.begin(), func_def_stmt.arg_list_type_ident.end(), std::back_inserter(args_semantic), [&](const std::pair<Token, Token>& type_ident) {
		auto* type = m_scopes->get_meta_type(type_ident.first.text);
		if (type == nullptr) { Debug("WROOOONG, type is not known"); abort(); }
		return type;
		});

	auto ret_type = m_scopes->get_meta_type(func_def_stmt.ret_type.text);
	if (ret_type == nullptr) {
		Debug("Wrong return type");
		abort(); 
	}

	auto func = new Function(func_def_stmt.name.text, std::move(args_semantic), ret_type);
	bool success = m_scopes->add(func);
	func_def_stmt.sem_function = func;
	if (!success) {
		Debug("Function already present");  abort();
	}


	m_scopes->expand();
	for (auto& stmt : func_def_stmt.body)
	{
		stmt->accept(*this);
	}
	m_scopes->ascend();
}


void DeclarationsCollector::visit(IdentPattern& ident)
{

}

void DeclarationsCollector::visit(IdentExpr& ident)
{
}
void DeclarationsCollector::visit(FuncCallExpr& func_call_expr)
{
}
void DeclarationsCollector::visit(FloatLiteralExpr& lit)
{
}
void DeclarationsCollector::visit(IntegerLiteralExpr& lit)
{
}
void DeclarationsCollector::visit(BinOpExpr& bin_op)
{
}
void DeclarationsCollector::visit(PrefixOpExpr& pre_op)
{
}
void DeclarationsCollector::visit(PostfixOpExpr& post_op)
{
}
void DeclarationsCollector::visit(InferredDeclStmt& decl_inferred)
{
}
void DeclarationsCollector::visit(ReturnStmt& ret_stmt)
{
}
void DeclarationsCollector::visit(ExprStmt& expr_stmt)
{
}
