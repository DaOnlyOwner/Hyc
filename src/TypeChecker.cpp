#include "TypeChecker.h"
#include "DebugPrint.h"

void TypeChecker::visit(FloatLiteralExpr& lit)
{
	// TODO: Don't use Double here.
	Primitive* primitive = m_scopes->get_primitive_type(Primitive::Specifier::Double);
	assert(primitive != nullptr);
	ret(static_cast<Type*>(primitive));
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	Primitive* primitive = m_scopes->get_primitive_type(lit.specifier);
	assert(primitive!=nullptr);
	ret(static_cast<Type*>(primitive));
}

void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto* typeLh = get(bin_op.lh);
	auto* typeRh = get(bin_op.rh);

	BinaryOperator* bin_operator = m_scopes->get_binary_operator(BinaryOperator::from_token_specifier(bin_op.op.type), *typeLh, *typeRh);

	if (bin_operator == nullptr)
	{
		auto descr = Error::FromToken(bin_op.op);
		descr.Message = fmt::format("The binary operator '{}' is not defined for types lh '{}' and rh '{}'", bin_op.op.text, typeLh->get_name(), typeRh->get_name());
		Error::SemanticError(descr);
		ret(&error_type);
		return;
	}
	ret(bin_operator->return_type);
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	auto* type = get(pre_op.lh);
	UnaryOperator* unary_operator = m_scopes->get_unary_operator(UnaryOperator::from_token_specifier(pre_op.op.type), *type);

	if (unary_operator == nullptr)
	{
		auto descr = Error::FromToken(pre_op.op);
		descr.Message = fmt::format("The prefix operator '{}' is not defined for type '{}'", pre_op.op.text, type->get_name());
		Error::SemanticError(descr);
		ret(&error_type);
		return;
	}

	ret(unary_operator->return_type);
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
	assert(false);
}

void TypeChecker::visit(InferredDefStmt& def_inferred)
{
	auto* expr_type = get(def_inferred.expr);
	def_inferred.type = expr_type;
	Variable* var = m_scopes->add(Variable(def_inferred.name.text, def_inferred.type));
	if (var == nullptr)
	{
		auto descr = Error::FromToken(def_inferred.name);
		descr.Message = fmt::format("The variable '{}' is already defined", def_inferred.name.text);
		Error::SemanticError(descr);
	}
}

void TypeChecker::visit(IdentExpr& ident)
{
	auto* var = m_scopes->get_var(ident.ident.text);
	if (var == nullptr) 
	{ 
		auto descr = Error::FromToken(ident.ident);
		descr.Message = fmt::format("The variable '{}' is not defined", ident.ident.text);
		Error::SemanticError(descr);
		ret(&error_type);
		return;
	}
	ret(static_cast<Type*>(var->type));
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	m_scopes->descend_next();
	for (int i = 0; i < namespace_stmt.stmts.size(); i++)
	{
		auto& stmt = namespace_stmt.stmts[i];
		stmt->accept(*this);
	}
	m_scopes->ascend();
}

void TypeChecker::visit(FuncCallExpr& func_call_expr)
{
	Function* func = m_scopes->get_func(func_call_expr.name.text, [&](const Function& func)
		{
			bool succ = true;
			if (func.arguments.size() != func_call_expr.arg_list.size()) return false;
			for (int i = 0; i < func.arguments.size(); i++)
			{
				auto overload_type = func.arguments[i];
				auto& expr_in_arg = func_call_expr.arg_list[i];
				auto type_in_arg = get(expr_in_arg);
				if (type_in_arg != overload_type) return false;
			}
			return true;
		});

	if (func == nullptr)
	{
		auto descr = Error::FromToken(func_call_expr.name);
		descr.Message = fmt::format("No viable overload in the call to '{}' found", func_call_expr.name.text);

		m_scopes

		func_call_expr.sem_function = &error_function;
		Error::SemanticError(descr);
		ret(&error_type);
		return;
	}

	func_call_expr.sem_function = func;

	ret(func->return_type);
	return;
}

void TypeChecker::visit(FuncDefStmt& func_def_stmt)
{
	m_current_func = &func_def_stmt;
	m_scopes->descend_next();

	for (int i = 0; i<func_def_stmt.arg_list_type_ident.size(); i++)
	{
		auto& func_arg = func_def_stmt.arg_list_type_ident[i];
		auto* type_sem = func_def_stmt.sem_function->arguments[i];
		m_scopes->add(Variable(func_arg.second.text, type_sem));
	}

	for (auto& stmt : func_def_stmt.body)
	{
		stmt->accept(*this);
	}
	m_scopes->ascend();
}

void TypeChecker::visit(ReturnStmt& ret_stmt)
{
	auto expr_type = get(ret_stmt.returned_expr);
	// This is good for now, until I implement type hierachies.
	if (m_current_func->sem_function->return_type != expr_type)
	{
		auto descr = Error::FromToken(ret_stmt.return_kw);
		descr.Message = fmt::format("The returned type '{}' and the function return type '{}' do not match", expr_type->get_name(), m_current_func->sem_function->return_type->get_name());
		Error::SemanticError(descr);
	}
}

void TypeChecker::visit(ExprStmt& expr_stmt)
{
	expr_stmt.expr->accept(*this);
}

void TypeChecker::visit(DefStmt& def_stmt)
{
	auto expr_type = get(def_stmt.expr);
	auto actual_type = m_scopes->get_meta_type(def_stmt.type_tkn.text);
	if (actual_type == nullptr)
	{
		auto descr = Error::FromToken(def_stmt.type_tkn);
		descr.Message = fmt::format("The declared type '{}' is undefined", def_stmt.type_tkn.text);
		Error::SemanticError(descr);
		return;
	}

	def_stmt.type = actual_type;

	if (actual_type != expr_type)
	{
		auto descr = Error::FromToken(def_stmt.type_tkn);
		descr.Message = fmt::format("The type of the expression '{}' and the declared type '{}' do not match", expr_type->get_name(), actual_type->get_name());
		Error::SemanticError(descr);
	}

	Variable* var = m_scopes->add(Variable(def_stmt.name.text, actual_type));
	if (var == nullptr)
	{
		auto descr = Error::FromToken(def_stmt.name);
		descr.Message = fmt::format("The variable '{}' is already defined", def_stmt.name.text);
		Error::SemanticError(descr);
	}
}
