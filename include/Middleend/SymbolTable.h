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

	bool add(CollectionStmt* cs);

	bool add(FuncDefStmt* fn);
	bool add(CollectionStmt* for_coll, DeclStmt* decl, size_t idx);
	bool add(DeclStmt* decl);

	CollectionStmt* get_type(const std::string& name) const;

	DeclStmt* get_decl_for(CollectionStmt* bt, const std::string& name);

	size_t get_decl_idx_for(CollectionStmt* cs, const std::string& name);

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

private:
	struct member
	{
		DeclStmt* stmt;
		size_t idx;
	};

	std::unordered_map<std::string, CollectionStmt*> collections;
	std::unordered_map<CollectionStmt*, std::unordered_map<std::string, member>> decl_in_collection;
	// name maps to overloads
	std::unordered_map<std::string, std::vector<FuncDefStmt*>> functions;

	std::unordered_map<std::string, DeclStmt*> variables;
};