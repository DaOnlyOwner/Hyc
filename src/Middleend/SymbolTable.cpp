#include "SymbolTable.h"
#include "TypeCreator.h"
#include <limits>

bool SymbolTable::add(CollectionStmt* cs)
{
	return collections.insert({ cs->name.text,cs }).second;
}

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

bool SymbolTable::add(CollectionStmt* for_coll, DeclStmt* decl, size_t idx)
{
	auto it = decl_in_collection.find(for_coll);
	if (it == decl_in_collection.end())
	{
		decl_in_collection[for_coll] = { {decl->name.text,{decl,idx}} };
		return true;
	}
	else
	{
		auto& coll_decls = it->second;
		auto succ = coll_decls.insert({ decl->name.text,{decl,idx} });
		return succ.second;
	}

}

bool SymbolTable::add(DeclStmt* decl)
{
	return variables.insert({ decl->name.text,decl}).second;
}

CollectionStmt* SymbolTable::get_type(const std::string& name) const
{
	auto it = collections.find(name);
	if (it != collections.end()) return it->second;
	return nullptr;
}

DeclStmt* SymbolTable::get_decl_for(CollectionStmt* bt, const std::string& name)
{
	auto it = decl_in_collection.find(bt);
	if(it == decl_in_collection.end()) return nullptr; 
	auto decl_it = it->second.find(name);
	if (decl_it == it->second.end()) return nullptr;
	else return decl_it->second.stmt;
}

size_t SymbolTable::get_decl_idx_for(CollectionStmt* cs, const std::string& name)
{
	auto it = decl_in_collection.find(cs);
	if (it == decl_in_collection.end()) return std::numeric_limits<size_t>::max();
	auto decl_it = it->second.find(name);
	if (decl_it == it->second.end()) return std::numeric_limits<size_t>::max();
	else return decl_it->second.idx;
}

DeclStmt* SymbolTable::get_variable(const std::string& name)
{
	auto it = variables.find(name);
	if (it != variables.end()) return it->second;
	return nullptr;
}
