#include "CodePaster.h"
#include <iterator>
#include <cassert>
#include "Messages.h"

std::string get_str(const std::string& name, const std::vector<uptr<TypeSpec>>& to_paste)
{
	std::string gl = "<";
	assert(!to_paste.empty());
	gl += to_paste[0]->as_str();
	for (int i = 1; i < to_paste.size(); i++)
	{
		auto& ts = to_paste[i]->as_str();
		gl += "," + ts;
	}
	gl += ">";
	return name + gl;
}

void CodePaster::visit(FuncDeclStmt& stmt)
{
	// TODO
}

void CodePaster::visit(DeclStmt& stmt)
{
	auto new_type_spec = mv(get(stmt.type_spec));
	stmt.type_spec = mv(new_type_spec);
}

void CodePaster::visit(BaseTypeSpec& bt)
{
	auto it = std::find_if(must_replace->begin(), must_replace->end(), [&](auto& gi) {return gi.name.text == bt.name.text; });
	if (it != must_replace->end())
	{
		if (!bt.generic_list.empty())
		{
			Messages::inst().trigger_code_paster_e1(bt.name);
			return;
		}
		int arg = std::distance(it,must_replace->end())-1;
		assert(arg < to_paste.size());
		auto p = to_paste[arg]->clone();
		p->push_inner(mv(bt.inner));
		RETURN(mv(p));
	}
	for (auto& arg : bt.generic_list)
	{
		arg->accept(*this); // Paste the generic types now
	}
}

void CodePaster::visit(CollectionStmt& stmt)
{
	auto pasted = uptr<CollectionStmt>(dynamic_cast<CollectionStmt*>(stmt.clone().release()));
	pasted->name.text = get_str(stmt.name.text, to_paste);
	pasted->generic_params.clear();
	must_replace = &stmt.generic_params;
	for (auto& p : pasted->stmts)
	{
		p->accept(*this);
	}
	auto* ptr = pasted.get();
	bool succ = scopes.add(ptr);
	// If no success we have already pasted
	if (succ)
		top_level.stmts.push_back(std::move(pasted));
}
