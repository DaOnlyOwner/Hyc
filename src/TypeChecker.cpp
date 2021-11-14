#include "TypeChecker.h"
#include "DebugPrint.h"
#include "Operators.h"

namespace
{
	std::string get_function_proto(const Function& func)
	{
		std::string type_args = "";
		if (func.arguments.size() > 0)
		{
			type_args += fmt::format("{}", func.arguments[0].as_str());
			for (int i = 1; i < func.arguments.size(); i++)
			{
				type_args += fmt::format(", {}", func.arguments[i].as_str());
			}
		}

		return fmt::format("{} {} ({})", func.return_type.as_str(), func.name, type_args);
	}

	std::string get_arg_types(const FuncCallExpr& expr)
	{
		std::string type_args = "";
		if (expr.arg_list.size() > 0)
		{
			type_args += fmt::format("{}", expr.arg_list[0]->sem_type.as_str());
			for (int i = 1; i < expr.arg_list.size(); i++)
			{
				type_args += fmt::format(", {}", expr.arg_list[i]->sem_type.as_str());
			}
		}
		return type_args;
	}
}

void TypeChecker::visit(FloatLiteralExpr& lit)
{
	// TODO: Don't use Double here.
	Type primitive = m_scopes->get_primitive_type(Primitive::Specifier::Double);
	lit.sem_type = primitive;
	RETURN(std::move(primitive));
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	Type primitive = m_scopes->get_primitive_type(lit.eval_res.Spec);
	lit.sem_type = primitive;
	RETURN(std::move(primitive));
}

void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto typeLh = get(bin_op.lh);
	auto& typeRh = get(bin_op.rh);
	ConversionType ctypeLh = ConversionType::NeedsCasting;
	ConversionType ctypeRh = ConversionType::NeedsCasting;
	
	// TODO: Pointer + integer 


	// TODO: Separate pass that desugares default vals
	Function* bin_operator = m_scopes->get_func(TokenToOperatorStr(bin_op.op), [&](const auto& fn) {
		if (fn.arguments.size() != 2) return false;

		// Call the function with the correct parameters, no implicit casts here
		return (clh == ConversionType::Same && crh == ConversionType::Same)
		});
	
	if (bin_operator == nullptr)
	{
		auto descr = Error::FromToken(bin_op.op);
		descr.Message = fmt::format("The binary operator '{}' is not defined for types lh '{}' and rh '{}'", bin_op.op.text, typeLh.as_str(), typeRh.as_str());
		Error::SemanticError(descr);
		bin_op.sem_type = error_type;
		RETURN(error_type);
	}
	bin_op.sem_type = bin_operator->return_type;
	RETURN(bin_operator->return_type);
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	auto& type = get(pre_op.lh);
	Function* unary_operator = m_scopes->get_func(TokenToOperatorStr(pre_op.op), [&](const auto& fn) {
		if (fn.arguments.size() != 1) return false;
		ConversionType clh = type.get_conversion_into(fn.arguments[0]);
		return (clh == ConversionType::Same)
		});

	if (unary_operator == nullptr)
	{
		auto descr = Error::FromToken(pre_op.op);
		descr.Message = fmt::format("The prefix operator '{}' is not defined for type '{}'", pre_op.op.text, type.as_str());
		Error::SemanticError(descr);
		pre_op.sem_type = error_type;
		RETURN(error_type);
	}
	pre_op.sem_type = unary_operator->return_type;
	RETURN(unary_operator->return_type);
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
	assert(false);
}

void TypeChecker::visit(InferredDefStmt& def_inferred)
{
	auto& expr_type = get(def_inferred.expr);
	def_inferred.type = expr_type;
	Variable* var = m_scopes->add(new Variable(def_inferred.name.text, expr_type));
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
		ident.sem_type = error_type;
		RETURN(error_type);
	}
	ident.sem_type = var->type;
	RETURN(var->type);
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	m_scopes->descend_next();
	for (int i = 0; i < namespace_stmt.stmts.size(); i++)
	{
		auto& stmt = namespace_stmt.stmts[i];
		stmt->accept(*this);
	}
}

void TypeChecker::visit(FuncCallExpr& func_call_expr)
{
	for (int i = 0; i < func_call_expr.arg_list.size(); i++)
	{
		auto& expr_in_arg = func_call_expr.arg_list[i];
		auto type_in_arg = get(expr_in_arg);
		func_call_expr.arg_list[i]->sem_type = type_in_arg;
	}

	Function* func = m_scopes->get_func(func_call_expr.name.text, [&](const Function& func)
		{
			bool succ = true;
			if (func.arguments.size() != func_call_expr.arg_list.size()) return false;
			for (int i = 0; i < func.arguments.size(); i++)
			{
				auto overload_type = func.arguments[i];
				auto type_in_arg = func_call_expr.arg_list[i]->sem_type;
				if (type_in_arg != overload_type) return false;
			}
			return true;
		});

	if (func == nullptr)
	{
		auto descr = Error::FromToken(func_call_expr.name);
		descr.Message = fmt::format("No viable overload in the call to '{}' found", func_call_expr.name.text);

		auto funcs = m_scopes->get_all_funcs(func_call_expr.name.text);
		std::string hint = "Overloads are: \n";
		for (int i = 0; i<funcs.size(); i++)
		{
			hint += fmt::format("{}. {}\n", i+1, get_function_proto(funcs[i]));
		}

		hint += fmt::format("The function was called with types '({})'", get_arg_types(func_call_expr));
		descr.Hint = hint;
		func_call_expr.sem_function = &error_function;
		func_call_expr.sem_type = error_type;
		Error::SemanticError(descr);
		RETURN(error_type);
		return;
	}

	func_call_expr.sem_function = func;

	RETURN(func->return_type);
}

void TypeChecker::visit(FuncDefStmt& func_def_stmt)
{
	m_scopes->descend_next();
	m_current_func = &func_def_stmt;

	for (int i = 0; i<func_def_stmt.arg_list_type_ident.size(); i++)
	{
		auto& func_arg = func_def_stmt.arg_list_type_ident[i];
		auto type_sem = func_def_stmt.sem_function->arguments[i];
		m_scopes->add(new Variable(func_arg.second.text, std::move(type_sem)));
	}

	// TODO: test for void
	return_stmt_in_func = false;
	for (auto& stmt : func_def_stmt.body)
	{
		stmt->accept(*this);
	}
}

// TODO: void
void TypeChecker::visit(ReturnStmt& ret_stmt)
{
	auto& expr_type = get(ret_stmt.returned_expr);

	auto conv = expr_type.get_conversion_into(m_current_func->sem_function->return_type,*m_scopes);
	if (conv == ConversionType::ImplicitCasting)
	{
		auto ice = std::make_unique<ImplicitCastExpr>(std::move(ret_stmt.returned_expr), expr_type);
		ret_stmt.returned_expr = std::move(ice);
	}

	if (conv == ConversionType::NeedsCasting)
	{
		auto descr = Error::FromToken(ret_stmt.return_kw);
		descr.Message = fmt::format("The returned type '{}' and the function return type '{}' do not match", expr_type.as_str(), m_current_func->sem_function->return_type.as_str());
		Error::SemanticError(descr);
	}
	return_stmt_in_func = true;
}

void TypeChecker::visit(ExprStmt& expr_stmt)
{
	expr_stmt.expr->accept(*this);
}

void TypeChecker::visit(DefStmt& def_stmt)
{
	auto expr_type = get(def_stmt.expr);
	auto actual_type = m_scopes->get_type(def_stmt.type_tkn.text);
	if (actual_type == nullptr)
	{
		auto descr = Error::FromToken(def_stmt.type_tkn);
		descr.Message = fmt::format("The declared type '{}' is undefined", def_stmt.type_tkn.text);
		Error::SemanticError(descr);
		return;
	}

	def_stmt.type = *actual_type;
	auto ct = expr_type.get_conversion_into(*actual_type, *m_scopes);
	if (ct == ConversionType::NeedsCasting)
	{
		auto descr = Error::FromToken(def_stmt.type_tkn);
		descr.Message = fmt::format("The type of the expression '{}' and the declared type '{}' do not match", expr_type.as_str(), actual_type->as_str());
		Error::SemanticError(descr);
	}

	else if (ct == ConversionType::ImplicitCasting)
	{
		auto ice = std::make_unique<ImplicitCastExpr>(std::move(def_stmt.expr), *actual_type);
	}

	Variable* var = m_scopes->add(new Variable(def_stmt.name.text, *actual_type));
	if (var == nullptr)
	{
		auto descr = Error::FromToken(def_stmt.name);
		descr.Message = fmt::format("The variable '{}' is already defined", def_stmt.name.text);
		Error::SemanticError(descr);
	}
}

void TypeChecker::visit(PointerTypeSpec& pt_spec)
{
}

void TypeChecker::visit(BaseTypeSpec& bt_spec)
{
}

void TypeChecker::visit(ArrayTypeSpec& at_spec)
{
}

void TypeChecker::visit(ImplicitCastExpr& ice)
{
}
