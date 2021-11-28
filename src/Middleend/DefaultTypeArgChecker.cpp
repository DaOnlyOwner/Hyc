#include "DefaultTypeArgChecker.h"
#include <unordered_set>

void DefaultTypeArgChecker::visit(CollectionStmt& ud)
{
	bool ended = false;
	std::unordered_set<std::string> types;
	for (int i = ud.generic_params.size() - 1; i >= 0; i--)
	{
		auto& dt = ud.generic_params[i].default_type;

		// Check if the default type is differs from the other generic parameters
		for (auto& arg : ud.generic_params)
		{
			must_not_contain = &arg.name.text;
			Token* tk;
			if ((tk=get(dt)) != nullptr)
			{
				auto descr = Error::FromToken(*tk);
				descr.Message = fmt::format("The default type of generic type parameter '{}' depends on type '{}' which is also a specified generic type parameter",
					ud.generic_params[i].name,arg.name.text);
				Error::SemanticError(descr);
			}
		}
		types.insert(ud.generic_params[i].name.text);
		if (dt == nullptr)
		{
			ended = true;
		}
		else
		{
			if (ended)
			{
				auto descr = Error::FromToken(ud.name);
				descr.Message = fmt::format("Default type argument of type parameter '{}' of '{}' '{}' is not at the end of the generic type list", ud.generic_params[i].name.text,
					ud.get_collection_type(), ud.name.text);
				Error::SemanticError(descr);
			}
		}
	}

	if (types.size() < ud.generic_params.size())
	{
		auto descr = Error::FromToken(ud.name);
		descr.Message = fmt::format("For {} '{}': Two or more generic type parameter have the same name", ud.get_collection_type(), ud.name);
		Error::SemanticError(descr);
	}

}

void DefaultTypeArgChecker::visit(BaseTypeSpec& bts)
{
	assert(false);
}

/*
void DefaultTypeArgChecker::visit(NamespaceStmt& ns)
{
	for (auto& stmt : ns.stmts) stmt->accept(*this);
}

void DefaultTypeArgChecker::visit(FuncDefStmt& func_def)
{
	for (auto& decl : func_def.decl->arg_list) decl->accept(*this);
	for (auto& b : func_def.body) b->accept(*this);
}


void DefaultTypeArgChecker::visit(IfStmt& if_stmt)
{
	for (auto& p : if_stmt.if_stmts) p->accept(*this);
	for (auto& elif : if_stmt.all_elif_stmts)
	{
		for (auto& p : elif) p->accept(*this);
	}
	for (auto& p : if_stmt.else_stmts) p->accept(*this);
}


void DefaultTypeArgChecker::visit(WhileStmt& while_stmt)
{
	for (auto& p : while_stmt.stmts) p->accept(*this);
}

void DefaultTypeArgChecker::visit(ForStmt& for_stmt)
{
	for_stmt.decl_stmt->accept(*this);
	for (auto& p : for_stmt.stmts) p->accept(*this);
}


void DefaultTypeArgChecker::visit(MatchStmt& match)
{
	for (auto& case_ : match.match_cases)
	{
		for (auto& p : case_.body) p->accept(*this);
	}
}

void DefaultTypeArgChecker::visit(DeclStmt& decl_stmt)
{
	decl_stmt.type_spec->accept(*this);
}

void DefaultTypeArgChecker::visit(BaseTypeSpec& bt)
{
	// Base case
	if (bt.generic_list.empty())
	{
		auto defined = scopes.get_type(bt.name.text);
		if (defined == nullptr)
		{
			auto descr = Error::FromToken(bt.name);
			descr.Message = fmt::format("The type '{}' is undefined", bt.name.text);
			Error::SemanticError(descr);
			return;
		}

		if (defined->generic_params.size() < bt.generic_list.size())
		{
			auto descr = Error::FromToken(bt.name);
			descr.Message = fmt::format("The type '{}' was given more generic types for instantiation than the definition specifies.", bt.as_str());
			descr.Hint = fmt::format("In the instantiation the type was given '{}' generic parameters, while the definition has only '{}'", bt.generic_list.size(), defined->generic_params.size());
			Error::SemanticError(descr);
			return;
		}

		for (int i = 0; i<defined->generic_params.size(); i++)
		{
			
		}

	}



}
*/

void check_default_type_arg(NamespaceStmt& ns, Scopes& sc)
{
	DefaultTypeArgChecker dtac(sc);
	ns.accept(dtac);
}
