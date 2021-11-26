#include "CodePaster.h"
#include <iterator>
#include <cassert>

namespace
{
	std::string get_str(const std::string& name, const std::vector<std::string>& to_paste)
	{
		std::string gl = "<";
		assert(!to_paste.empty());
		gl += to_paste[0];
		for (int i = 1; i < to_paste.size(); i++)
		{
			auto& ts = to_paste[i];
			gl += "," + ts;
		}
		gl += ">";
		return name + gl;
	}
}


void CodePaster::visit(FuncDeclStmt& stmt)
{
	// TODO
}

void CodePaster::visit(DeclStmt& stmt)
{
	stmt.type_spec->accept(*this);
}

void CodePaster::visit(BaseTypeSpec& bt)
{
	auto it = std::find_if(must_replace->begin(), must_replace->end(), [&](auto& gi) {return gi.name.text == bt.name.text; });
	if (it != must_replace->end())
	{
		if (!bt.generic_list.empty())
		{
			auto descr = Error::FromToken(bt.name);
			descr.Message = fmt::format("type '{}': A generic type parameter must not have generic parameters specified", bt.name.text);
			Error::SemanticError(descr);
		}
		int arg = std::distance(it,must_replace->end());
		assert(arg < to_paste.size());
		auto& p = to_paste[arg];
		bt.name.text = p;
	}
}

void CodePaster::visit(StructDefStmt& stmt)
{
	auto pasted = uptr<StructDefStmt>(dynamic_cast<StructDefStmt*>(stmt.clone().release()));
	pasted->name.text = get_str(stmt.name.text, to_paste);
	pasted->generic_params.clear();
	must_replace = &stmt.generic_params;
	for (auto& s : pasted->stmts) s->accept(*this);
	top_level.stmts.push_back(std::move(pasted));
	scopes.at_root().add((CollectionStmt*)top_level.stmts.back().get());
}

void CodePaster::visit(UnionDefStmt& stmt)
{
	auto pasted = uptr<UnionDefStmt>(dynamic_cast<UnionDefStmt*>(stmt.clone().release()));
	pasted->name.text = get_str(stmt.name.text, to_paste);
	pasted->generic_params.clear();
	must_replace = &stmt.generic_params;
	for (auto& s : pasted->stmts) s->accept(*this);
	top_level.stmts.push_back(std::move(pasted));
	scopes.at_root().add((CollectionStmt*)top_level.stmts.back().get());
}
