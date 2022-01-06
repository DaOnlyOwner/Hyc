#include "DeclarationsCollectorFunctions.h"
#include <algorithm>
#include "Messages.h"

namespace
{
	std::string get_function_proto(FuncDefStmt& func_def_stmt)
	{
		std::string type_args = "";
		if (func_def_stmt.decl->arg_list.size() > 0)
		{
			type_args += fmt::format("{}", func_def_stmt.decl->arg_list[0].decl->type_spec->as_str());
			for (int i = 1; i < func_def_stmt.decl->arg_list.size(); i++)
			{
				type_args += fmt::format(", {}", func_def_stmt.decl->arg_list[i].decl->type_spec->as_str());
			}
		}

		return fmt::format("{} {} ({})", func_def_stmt.decl->ret_type->as_str(), func_def_stmt.decl->name.text, type_args);
	}
}

void DeclarationsCollectorFunctions::visit(NamespaceStmt& ns)
{
	for (auto& stmt : ns.stmts) stmt->accept(*this);
}

void DeclarationsCollectorFunctions::visit(FuncDefStmt& func_def_stmt)
{
	bool succ;
	if (func_def_stmt.is_operator)
		succ = scopes.add_op(&func_def_stmt);
	else 
		succ = scopes.add(&func_def_stmt);
	if (!succ) {
		auto proto = get_function_proto(func_def_stmt);
		if(func_def_stmt.is_operator)
			Messages::inst().trigger_5_e2(func_def_stmt.decl->name, proto);
		else
			Messages::inst().trigger_5_e1(func_def_stmt.decl->name, proto);
	}
}

void collect_funcs(NamespaceStmt& ns, Scopes& sc)
{
	DeclarationsCollectorFunctions dcf(sc, ns);
	ns.accept(dcf);
}