#include "CreateFuncArgsType.h"
#include "TypeCreator.h"

void CreateFuncArgsType::visit(NamespaceStmt& ns)
{
	scopes.descend();
	for (auto& stmt : ns.stmts) stmt->accept(*this);
	scopes.go_to_root();
}

void CreateFuncArgsType::visit(FuncDefStmt& def)
{
	auto [r,succ] = create_type(*def.decl->ret_type, scopes, ns);
	def.decl->ret_type->semantic_type = std::move(r);
	if (!succ)
	{
		auto descr = Error::FromToken(def.decl->ret_type->get_ident_token());
		descr.Message = fmt::format("In return type of function '{}'", def.decl->name.text);
		Error::Info(descr);
	}

	for (int i = 0; i<def.decl->arg_list.size(); i++)
	{
		auto& arg = def.decl->arg_list[i];
		auto [a, succ_a] = create_type(*arg.first, scopes, ns);
		if (!succ_a)
		{
			auto descr = Error::FromToken(arg.second);
			descr.Message = fmt::format("In type of argument '{}' of function '{}'", arg.second.text,def.decl->name.text);
			Error::Info(descr);
		}
		arg.first->semantic_type = std::move(a);
	}
}
