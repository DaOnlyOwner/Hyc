#include "DeclarationsCollectorFunctions.h"
#include <algorithm>
#include "TypeCreator.h"

namespace
{
	std::string get_function_proto(FuncDefStmt& func_def_stmt)
	{
		std::string type_args = "";
		if (func_def_stmt.decl->arg_list.size() > 0)
		{
			type_args += fmt::format("{}", func_def_stmt.decl->arg_list[0].first->as_str());
			for (int i = 1; i < func_def_stmt.decl->arg_list.size(); i++)
			{
				type_args += fmt::format(", {}", func_def_stmt.decl->arg_list[i].first->as_str());
			}
		}

		return fmt::format("{} {} ({})", func_def_stmt.decl->ret_type->as_str(), func_def_stmt.decl->name.text, type_args);
	}
}

void DeclarationsCollectorFunctions::visit(NamespaceStmt& ns)
{
	scopes.descend();
	for (auto& stmt : ns.stmts) stmt->accept(*this);
}

void DeclarationsCollectorFunctions::visit(FuncDefStmt& func_def_stmt)
{
	std::vector<Type> args_semantic;
	std::transform(func_def_stmt.decl->arg_list.begin(), func_def_stmt.decl->arg_list.end(), std::back_inserter(args_semantic), [&](std::pair<uptr<TypeSpec>, Token>& type_ident) {
		auto [type, succ] = create_type(*type_ident.first, scopes, ns);
		if (!succ)
		{
			return error_type;
		}
		return type;
		});

	auto [ret_type, succ] = create_type(*func_def_stmt.decl->ret_type, scopes, ns);
	if (!succ) {
		ret_type = error_type;
	}
	succ = scopes.add(&func_def_stmt);
	if (!succ) {
		auto descr = Error::FromToken(func_def_stmt.decl->name);
		auto proto = get_function_proto(func_def_stmt);
		descr.Message = fmt::format("The function '{}' is already defined.", proto);
		descr.Hint = "You have multiple definitions of the same function with the same arguments. Maybe change this function name? Note, that the return types don't play a role in detecting wether the function has been defined before, only the name and the arguments.";
		Error::SemanticError(descr);
	}
}