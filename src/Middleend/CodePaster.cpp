#include "CodePaster.h"
#include <iterator>
#include <cassert>
#include "Messages.h"
#include "UptrCast.h"
#include "CreateFuncArgsType.h"

std::string get_str(const std::string& name, const std::vector<uptr<TypeSpec>>& to_paste)
{
	std::string gl = "<";
	assert(!to_paste.empty());
	gl += to_paste[0]->as_str();
	for (int i = 1; i < to_paste.size(); i++)
	{
		auto ts = to_paste[i]->as_str();
		gl += "," + ts;
	}
	gl += ">";
	return name + gl;
}

void CodePaster::visit(FuncDefStmt& stmt)
{
	auto new_def = dynamic_unique_cast<FuncDefStmt>(stmt.clone());
	new_def->decl->generic_list.clear();
	new_def->decl->name.text = get_str(stmt.decl->name.text, to_paste);
	must_replace = &stmt.decl->generic_list;
	for (auto& p : new_def->decl->arg_list) p->accept(*this);
	auto new_type_spec = mv(get(new_def->decl->ret_type));
	new_def->decl->ret_type = mv(new_type_spec);
	for (auto& p : new_def->body) p->accept(*this);
	auto ptr = new_def.get();
	CreateFuncArgsType cfat(scopes, top_level);
	new_def->accept(cfat);
	bool succ = scopes.add(ptr);
	if (succ)
		top_level.stmts.push_back(std::move(new_def));
}

void CodePaster::visit(DeclStmt& stmt)
{
	if (!stmt.type_spec) return;
	auto new_type_spec = mv(get(stmt.type_spec));
	stmt.type_spec = mv(new_type_spec);
}

void CodePaster::visit(BaseTypeSpec& bt)
{
	// Find the bt.name in must_replace
	auto it = std::find_if(must_replace->begin(), must_replace->end(), [&](auto& gi) {return gi.name.text == bt.name.text; });
	// If the base type exists in must_replace
	if (it != must_replace->end())
	{
		if (!bt.generic_list.empty())
		{
			// The generic type is not generic itself
			Messages::inst().trigger_code_paster_e1(bt.name);
			return;
		}
		// Get the idx of the argument in to_paste and paste it.
		int arg = std::distance(must_replace->begin(), it);
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

void CodePaster::visit(TypeDefStmt& stmt)
{
	auto pasted = uptr<TypeDefStmt>(dynamic_cast<TypeDefStmt*>(stmt.clone().release()));
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
