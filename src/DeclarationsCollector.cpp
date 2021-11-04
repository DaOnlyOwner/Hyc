#include "DeclarationsCollector.h"
#include <algorithm>
#include "DebugPrint.h"
#include <iterator>


namespace
{
	std::string get_function_proto(FuncDefStmt& func_def_stmt)
	{
		std::string type_args = "";
		if (func_def_stmt.arg_list_type_ident.size() > 0)
		{
			type_args += fmt::format("{}", func_def_stmt.arg_list_type_ident[0].first.text);
			for (int i = 1; i < func_def_stmt.arg_list_type_ident.size(); i++)
			{
				type_args += fmt::format(", {}", func_def_stmt.arg_list_type_ident[i].first.text);
			}
		}

		return fmt::format("{} {} ({})", func_def_stmt.ret_type.text, func_def_stmt.name.text, type_args);
	}
}


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
		if (type == nullptr) 
		{ 
			std::string proto = get_function_proto(func_def_stmt);
			auto descr = Error::FromToken(type_ident.first);
			descr.Message = fmt::format("In function definition of '{}': The specified type '{}' of the ident type pair '{} {}' is unknown", proto, type_ident.first.text, type_ident.first.text, type_ident.second.text);
			descr.Hint = "You have not declared the specified type. Maybe the declaration comes after the use? If so you should put the declaration before you use the type";
			Error::SemanticError(descr);
			return (Type*)&error_type;
		}
		return type;
		});

	auto ret_type = m_scopes->get_meta_type(func_def_stmt.ret_type.text);
	if (ret_type == nullptr) {
		auto descr = Error::FromToken(func_def_stmt.ret_type);
		auto proto = get_function_proto(func_def_stmt);
		descr.Message = fmt::format("The return type '{}' of function '{}' is unknown", func_def_stmt.ret_type.text, proto);
		descr.Hint = "You have not declared the specified type. Maybe the declaration comes after the use? If so you should put the declaration before you use the type";
		Error::SemanticError(descr);
		ret_type = (Type*)&error_type;
	}

	Function func(func_def_stmt.name.text, std::move(args_semantic), ret_type);
	Function* f_ptr = m_scopes->add(std::move(func));
	func_def_stmt.sem_function = f_ptr;
	if (!f_ptr) {
		auto descr = Error::FromToken(func_def_stmt.name);
		auto proto = get_function_proto(func_def_stmt);
		descr.Message = fmt::format("The function '{}' is already defined.", proto);
		descr.Hint = "You have multiple definitions of the same function with the same arguments. Maybe change this function name? Note, that the return types don't play a role in detecting wether the function has been defined before, only the name and the arguments.";
		Error::SemanticError(descr);
	}

	m_scopes->expand();
	for (auto& stmt : func_def_stmt.body)
	{
		stmt->accept(*this);
	}
	m_scopes->ascend();
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
void DeclarationsCollector::visit(InferredDefStmt& decl_inferred)
{
}
void DeclarationsCollector::visit(ReturnStmt& ret_stmt)
{
}
void DeclarationsCollector::visit(ExprStmt& expr_stmt)
{
}

void DeclarationsCollector::visit(DefStmt& def_stmt)
{
}
