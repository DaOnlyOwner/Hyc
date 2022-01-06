#include "SymbolTable.h"
#include "TypeCreator.h"
#include <limits>
#include <algorithm>


bool SymbolTable::add(TypeDefStmt* cs)
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
			Type t1 = fn->decl->arg_list[i].decl->type;
			Type t2 = fn_->decl->arg_list[i].decl->type;
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

bool SymbolTable::add_op(FuncDefStmt* op)
{
	return operators.insert({ op->decl->name.text,op }).second;
}

bool SymbolTable::add(TypeDefStmt* for_coll, DeclStmt* decl, size_t idx)
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

bool SymbolTable::add(TypeDefStmt* td, UnionDeclStmt* udecl)
{
	auto it = union_decl_in_collection.find(td);
	if (it == union_decl_in_collection.end())
	{
		union_decl_in_collection[td] = { {udecl->decl_stmt->name.text,udecl} };
		return true;
	}
	else
	{
		auto& coll_decls = it->second;
		auto succ = coll_decls.insert({ udecl->decl_stmt->name.text,udecl });
		return succ.second;
	}
}

bool SymbolTable::add(DeclStmt* decl)
{
	return variables.insert({ decl->name.text,{var_idx++,decl} }).second;
}

bool SymbolTable::add(const std::string& name, llvm::Value* val)
{
	return llvm_variables.insert({ name,val }).second;
}

TypeDefStmt* SymbolTable::get_type(const std::string& name) const
{
	auto it = collections.find(name);
	if (it != collections.end()) return it->second;
	return nullptr;
}

DeclStmt* SymbolTable::get_decl_for(TypeDefStmt* bt, const std::string& name)
{
	auto it = decl_in_collection.find(bt);
	if(it == decl_in_collection.end()) return nullptr; 
	auto decl_it = it->second.find(name);
	if (decl_it == it->second.end()) return nullptr;
	else return decl_it->second.stmt;
}

UnionDeclStmt* SymbolTable::get_union_decl_for(TypeDefStmt* td, const std::string& name)
{
	auto it = union_decl_in_collection.find(td);
	if (it == union_decl_in_collection.end()) return nullptr;
	auto decl_it = it->second.find(name);
	if (decl_it == it->second.end()) return nullptr;
	else return decl_it->second;
}

size_t SymbolTable::get_decl_idx_for(TypeDefStmt* cs, const std::string& name)
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
	if (it != variables.end()) return it->second.second;
	return nullptr;
}

std::vector<std::pair<size_t,DeclStmt*>> SymbolTable::get_all_variables_reversed()
{
	std::vector<std::pair<size_t, DeclStmt*>> out;
	std::transform(variables.begin(), variables.end(), std::back_inserter(out), [](const auto& var) {
		return var.second;
		});

	std::sort(out.begin(), out.end(), [](const auto& a1, const auto& a2) {
		return a1.first <= a2.first;
		});
	return out;
}

llvm::Value* SymbolTable::get_value(const std::string& name)
{
	auto it = llvm_variables.find(name);
	if (it != llvm_variables.end()) return it->second;
	return nullptr;
}
