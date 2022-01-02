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
#include "llvm/IR/Value.h"
#include <map>

class SymbolTable
{
public:
	SymbolTable() = default;

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	SymbolTable(SymbolTable&& other) noexcept= default;
	SymbolTable& operator=(SymbolTable&& other) = default;

	bool add(TypeDefStmt* cs);

	bool add(FuncDefStmt* fn);
	bool add_op(FuncDefStmt* op);
	bool add(TypeDefStmt* td, DeclStmt* decl, size_t idx);
	bool add(TypeDefStmt* td, UnionDeclStmt* udecl);
	bool add(DeclStmt* decl);
	bool add(const std::string& name, llvm::Value*);

	TypeDefStmt* get_type(const std::string& name) const;

	DeclStmt* get_decl_for(TypeDefStmt* bt, const std::string& name);
	UnionDeclStmt* get_union_decl_for(TypeDefStmt* td, const std::string& name);

	size_t get_decl_idx_for(TypeDefStmt* cs, const std::string& name);

	DeclStmt* get_variable(const std::string& name);
	std::vector<std::pair<size_t, DeclStmt*>> get_all_variables_reversed();

	llvm::Value* get_value(const std::string& name);

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

	FuncDefStmt* get_op(const std::string& name)
	{
		auto it = operators.find(name);
		if (it == operators.end()) return nullptr;
		else return it->second;
	}

private:
	struct member
	{
		DeclStmt* stmt;
		size_t idx;
	};
	size_t var_idx = 0;
	std::unordered_map<std::string, TypeDefStmt*> collections;
	std::unordered_map<TypeDefStmt*, std::unordered_map<std::string, member>> decl_in_collection;
	// name maps to overloads
	std::unordered_map<std::string, std::vector<FuncDefStmt*>> functions;
	std::unordered_map<std::string, FuncDefStmt*> operators;

	std::unordered_map<std::string, std::pair<size_t,DeclStmt*>> variables;
	std::unordered_map<std::string, llvm::Value*> llvm_variables;

	std::unordered_map<TypeDefStmt*, std::unordered_map<std::string, UnionDeclStmt*>> union_decl_in_collection;
};