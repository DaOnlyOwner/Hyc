#include "DefaultTypeArgChecker.h"
#include <unordered_set>
#include "Messages.h"

void DefaultTypeArgChecker::visit(CollectionStmt& ud)
{
	bool ended = false;
	std::unordered_set<std::string> types;
	for (int i = ud.generic_params.size() - 1; i >= 0; i--)
	{
		auto& dt = ud.generic_params[i].default_type;

		// Check if the default type is differs from the other generic parameters
		if (dt)
		{
			for (auto& arg : ud.generic_params)
			{
				must_not_contain = &arg.name.text;
				Token* tk;
				if ((tk = get(dt)) != nullptr)
				{
					Messages::inst().trigger_2_e1(*tk, ud.generic_params[i].name.text, arg.name.text);
				}
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
				Messages::inst().trigger_2_e2(ud.name,ud.generic_params[i].name.text,
					ud.get_collection_type(), ud.name.text);
			}
		}
	}

	if (types.size() < ud.generic_params.size())
	{
		Messages::inst().trigger_2_e3(ud.name, ud.get_collection_type());
	}

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

void DefaultTypeArgChecker::visit(NamespaceStmt& ns)
{
	for (auto& p : ns.stmts) p->accept(*this);
}

void check_default_type_arg(NamespaceStmt& ns, Scopes& sc)
{
	DefaultTypeArgChecker dtac(sc);
	ns.accept(dtac);
}
