#include "SymbolTable.h"
#include "TypeCreator.h"

bool SymbolTable::add(FuncDefStmt* fn)
{
	auto it = functions.find(fn->decl->name.text);
	if (it == functions.end())
	{
		std::vector<FuncDefStmt*> vec = { fn };
		functions[fn->decl->name.text] = std::move(vec);
		return true;
	}

	auto& vec = it->second;
	if (std::find_if(vec.begin(), vec.end(), [&](auto& fn_) {
		// If we have different function arguments, we can differentiate between them.
		if (fn_->decl->arg_list.size() != fn->decl->arg_list.size()) return false;
		/*
		int a<T>(T arg);
		int a(int arg);
		What should a(3) call, the first function or the second? 
		That's the problem here.
		So that's why I don't allow two generic functions with the same name and same argument size
		*/
		if (!fn->decl->generic_list.empty() || !fn_->decl->generic_list.empty()) return true;
		bool same = true;
		for (int i = 0; i < fn->decl->arg_list.size(); i++)
		{
			Type t1 = fn->decl->arg_list[i]->type_spec->semantic_type;
			Type t2 = fn_->decl->arg_list[i]->type_spec->semantic_type;
			if (t1 != t2) same = false;
		}
		return same;

		}) == vec.end())
	{
		vec.push_back(fn);
		return true;
	}
	return false;
}

bool SymbolTable::add(BaseType* for_coll, DeclStmt* decl)
{
	auto it = decl_in_collection.find(for_coll);
	if (it == decl_in_collection.end())
	{
		decl_in_collection[for_coll] = { {decl->name.text,decl} };
		return true;
	}
	else
	{
		auto& coll_decls = it->second;
		auto succ = coll_decls.insert({ decl->name.text,decl });
		return succ.second;
	}

}
