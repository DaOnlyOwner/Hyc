#include "DefaultTypeArgChecker.h"
#include <unordered_set>
#include "Messages.h"


void DefaultTypeArgChecker::check(std::vector<GenericInfo>& gis, const Token& name, const std::string& t)
{
	bool ended = false;
	std::unordered_set<std::string> types;
	for (int i = gis.size() - 1; i >= 0; i--)
	{
		auto& dt = gis[i].default_type;

		// Check if the default type is differs from the other generic parameters
		if (dt)
		{
			for (auto& arg : gis)
			{
				must_not_contain = &arg.name.text;
				Token* tk;
				if ((tk = get(dt)) != nullptr)
				{
					Messages::inst().trigger_2_e1(*tk, gis[i].name.text, arg.name.text);
					error = true;
				}
			}
		}
		types.insert(gis[i].name.text);
		if (dt == nullptr)
		{
			ended = true;
		}
		else
		{
			if (ended)
			{
				Messages::inst().trigger_2_e2(name, gis[i].name.text,
					t, name.text);
				error = true;
			}
		}
	}

	if (types.size() < gis.size())
	{
		Messages::inst().trigger_2_e3(name, t);
	}
}

void DefaultTypeArgChecker::visit(CollectionStmt& ud)
{
	check(ud.generic_params, ud.name, ud.get_collection_type());
}

void DefaultTypeArgChecker::visit(BaseTypeSpec& bts)
{
	if (bts.name.text == *must_not_contain)
	{
		RETURN(&bts.name);
	}

	for (auto& p : bts.generic_list)
	{
		auto out = get(p);
		if (out != nullptr)
		{
			RETURN(out);
		}
	}
	RETURN(nullptr);
}

void DefaultTypeArgChecker::visit(FuncDefStmt& def)
{
	check(def.decl->generic_list, def.decl->name, "function"); 
}

void DefaultTypeArgChecker::visit(NamespaceStmt& ns)
{
	for (auto& p : ns.stmts) p->accept(*this);
}

bool check_default_type_arg(NamespaceStmt& ns, Scopes& sc)
{
	DefaultTypeArgChecker dtac(sc);
	ns.accept(dtac);
	return dtac.had_errors();
}
