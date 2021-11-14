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
			type_args += fmt::format("{}", func_def_stmt.arg_list_type_ident[0].first->as_str());
			for (int i = 1; i < func_def_stmt.arg_list_type_ident.size(); i++)
			{
				type_args += fmt::format(", {}", func_def_stmt.arg_list_type_ident[i].first->as_str());
			}
		}

		return fmt::format("{} {} ({})", func_def_stmt.ret_type->as_str(), func_def_stmt.name.text, type_args);
	}
}


DeclarationsCollector::DeclarationsCollector()
	:ValueStorage(this)
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
	std::vector<Type> args_semantic;
	std::transform(func_def_stmt.arg_list_type_ident.begin(), func_def_stmt.arg_list_type_ident.end(), std::back_inserter(args_semantic), [&](std::pair<uptr<TypeSpec>, Token>& type_ident) {
		auto type = get(type_ident.first);
		type.reverse();
		if (!m_scopes->is_type_defined(type)) 
		{ 
			std::string proto = get_function_proto(func_def_stmt);
			auto descr = Error::FromToken(type_ident.first->get_ident_token());
			descr.Message = fmt::format("In function definition of '{}': The specified type '{}' of the ident type pair '{} {}' is unknown", proto, type.as_str(), type.as_str(), type_ident.second.text);
			descr.Hint = "You have not declared the specified type. Maybe the declaration comes after the use? If so you should put the declaration before you use the type";
			Error::SemanticError(descr);
			return error_type;
		}
		return type;
		});

	auto& ret_type = get(func_def_stmt.ret_type);
	ret_type.reverse();
	if (!m_scopes->is_type_defined(ret_type)) {
		auto descr = Error::FromToken(func_def_stmt.ret_type->get_ident_token());
		auto proto = get_function_proto(func_def_stmt);
		descr.Message = fmt::format("The return type '{}' of function '{}' is unknown", ret_type.as_str(), proto);
		descr.Hint = "You have not declared the specified type. Maybe the declaration comes after the use? If so you should put the declaration before you use the type";
		Error::SemanticError(descr);
		ret_type = error_type;
	}
	Function* func = new Function(func_def_stmt.name.text, std::move(args_semantic), std::move(ret_type));
	Function* f_ptr = m_scopes->add(func);
	func_def_stmt.sem_function = f_ptr;
	if (!f_ptr) {
		auto descr = Error::FromToken(func_def_stmt.name);
		auto proto = get_function_proto(func_def_stmt);
		descr.Message = fmt::format("The function '{}' is already defined.", proto);
		descr.Hint = "You have multiple definitions of the same function with the same arguments. Maybe change this function name? Note, that the return types don't play a role in detecting wether the function has been defined before, only the name and the arguments.";
		Error::SemanticError(descr);
		func_def_stmt.sem_function = &error_function;
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

void DeclarationsCollector::visit(PointerTypeSpec& pt_spec)
{
	if (pt_spec.inner != nullptr)
	{
		auto& type = get(pt_spec.inner);
		type.promote_pointer();
		RETURN(std::move(type));
	}

	Type t;
	t.promote_pointer();
	RETURN(std::move(t));
}

void DeclarationsCollector::visit(BaseTypeSpec& bt_spec)
{
	if (bt_spec.inner)
	{
		auto& type = get(bt_spec.inner);
		type.promote_base(bt_spec.name.text); 
		return;
	}
	RETURN(Type(bt_spec.name.text));
}

void DeclarationsCollector::visit(ArrayTypeSpec& at_spec)
{
	uint64_t amount;
	if (at_spec.amount->eval_res.Spec != Primitive::Specifier::uint)
	{
		auto descr = Error::FromToken(at_spec.amount->integer_literal);
		descr.Message = fmt::format("array size {} specifier is not of type uint",at_spec.amount->integer_literal.text);
		Error::SyntacticalError(descr);
	}
	amount = at_spec.amount->eval_res.amount_uint;
	if(at_spec.inner)
	{
		auto& type = get(at_spec.inner);
		type.promote_array(amount);
		return;
	}
	RETURN(Type(amount));
}

void DeclarationsCollector::visit(ImplicitCastExpr& ice)
{
}
