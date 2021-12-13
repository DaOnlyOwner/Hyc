#pragma once

#include <string>
#include "AtomicType.h"
#include "Primitive.h"
#include <algorithm>
#include <optional>
#include <memory>
#include "DebugPrint.h"
#include <iterator>
#include "Ast.h"
#include <unordered_map>

class SymbolTable
{
public:
	SymbolTable() = default;

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	SymbolTable(SymbolTable&& other) noexcept= default;
	SymbolTable& operator=(SymbolTable&& other) = default;

	/*Variable* add(Variable* var) {
		return insert_into(variables, var, var->name);
	}
	Function* add(Function* fn);
	Type* add(Type* mt) { 
		return insert_into(types, mt, mt->as_str()); // This is just the base case e.g. struct A -> "A" gets inserted, never a pointer type etc.
	}
	Variable* get_var(const std::string& name)
	{
		auto it = variables.find(name);
		if (it == variables.end()) return nullptr;
		return it->second.get();
	}*/

	bool add(CollectionStmt* cs);

	bool add(FuncDefStmt* fn);
	bool add(CollectionStmt* for_coll, DeclStmt* decl);
	bool add(DeclStmt* decl);
	//bool add_to_existing(const std::string&, class AllocaInst*);

	CollectionStmt* get_type(const std::string& name) const;

	DeclStmt* get_decl_for(CollectionStmt* bt, const std::string& name);

	DeclStmt* get_variable(const std::string& name);

	template<typename Pred>
	FuncDefStmt* get_func(const std::string& name, Pred pred)
	{
		auto it = functions.find(name);
		if (it == functions.end()) return nullptr;
		auto& vars = it->second;
		auto it2 = std::find_if(vars.begin(), vars.end(), [&](FuncDefStmt* func) {return pred(*func->decl); });
		if (it2 != vars.end()) return *it2;
		return nullptr;
	}

	//class AllocaInst* get_alloca_inst(const std::string& name);

	/*
	std::vector<Function> get_funcs(const std::string& name)
	{
		auto it = functions.find(name);
		if (it == functions.end()) return {};
		std::vector<Function> out;
		std::transform(it->second.begin(), it->second.end(), std::back_inserter(out), [](auto& func) {return *func; });
		return out;
	}*/

private:

	std::unordered_map<std::string, CollectionStmt*> collections;
	std::unordered_map<CollectionStmt*, std::unordered_map<std::string, DeclStmt*>> decl_in_collection;
	// name maps to overloads
	std::unordered_map<std::string, std::vector<FuncDefStmt*>> functions;

	struct named_values
	{
		DeclStmt* decl;
		class AllocaInst* inst;
	};

	std::unordered_map<std::string, DeclStmt*> variables;
};