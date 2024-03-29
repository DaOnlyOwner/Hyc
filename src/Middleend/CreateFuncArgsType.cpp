#include "CreateFuncArgsType.h"
#include "TypeCreator.h"
#include "Messages.h"
#include "Mangling.h"

void CreateFuncArgsType::visit(NamespaceStmt& ns)
{
	size_t size_before_pasting = ns.stmts.size();
	for (size_t i = 0; i < size_before_pasting; i++)
	{
		auto& stmt = ns.stmts[i];
		stmt->accept(*this);
	}
}

void CreateFuncArgsType::visit(FuncDefStmt& def)
{
	// Don't instantiate generic func defs
	if (!def.decl->generic_list.empty()) return;
	auto [r,succ] = create_type(*def.decl->ret_type, scopes, ns);
	def.decl->ret_type->semantic_type = std::move(r);
	if (!succ)
	{
		Messages::inst().trigger_3_i1(def.decl->ret_type->get_ident_token(), def.decl->name.text);
	}

	for (int i = 0; i<def.decl->arg_list.size(); i++)
	{
		auto& arg = def.decl->arg_list[i];
		// Don't try to create a type for a generic parameter.
		//if (std::find_if(def.decl->generic_list.begin(), def.decl->generic_list.end(), [&](GenericInfo& t) {return t.name.text == arg->type_spec->as_str(); }) != def.decl->generic_list.end())
		//	continue;
		auto [a, succ_a] = create_type(*arg.decl->type_spec, scopes, ns);
		if (!succ_a)
		{
			Messages::inst().trigger_3_i2(arg.decl->name, def.decl->name.text);
		}
		arg.decl->type_spec->semantic_type = std::move(a);
		arg.decl->type = arg.decl->type_spec->semantic_type;
	}

	if (def.is_operator)
	{
		auto& decl = *def.decl;
		// Make new name
		decl.name.text = mangle(decl);
	}
}

void create_func_args_type(Scopes& sc, NamespaceStmt& ns)
{
	CreateFuncArgsType cfat(sc, ns);
	ns.accept(cfat);
}
