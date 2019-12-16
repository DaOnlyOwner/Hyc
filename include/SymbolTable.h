#pragma once

#include <string>
#include "IndexedHashmap.h"
#include "MetaType.h"
#include "UID.h"
#include "Primitive.h"

struct Function 
{
	std::string name;
	UID return_type;
	std::vector<UID> arguments;
	bool operator==(const Function& other) { name == other.name && arguments == other.arguments; }
	bool operator!=(const Function& other) { !(*this == other); }
};

struct Variable
{
	std::string name;
	UID type;
	bool operator==(const Variable& other) { name == other.name && type == other.type; }
	bool operator!=(const Variable& other) { !(*this == other); }
};


class SymbolTable
{
public:
	SymbolTable();

	bool add(Variable&& var) { return m_variables.Add(var.name,std::move(var)) > 0; }
	bool add(Function&& fn);
	bool add(MetaType&& mt) { return m_meta_types.Add(mt.get_name(), std::move(mt)) > 0; }

	std::pair<Variable*, size_t> get_var(const std::string& name) { auto ret = m_variables.Get(name); return std::make_pair(ret.first, ret.second); }
	Variable& get_var(const UID& uid) { return m_variables.GetByIndex(uid.index); }

	std::pair<std::vector<Function>*,size_t> get_funcs(const std::string& name) { return m_functions.Get(name);}
	// TODO: Performance: predefined operations on predefined types (e.g. 1+3) should be in a static table for faster lookup. For example a static (and modified SymbolTable class) called predefined_symbols, that doesn't have string as key etc. 
	std::vector<Function>& get_funcs(const UID& uid) { return m_functions.GetByIndex(uid.index); }

	// TODO: Performance: When primitive type, first lookup in a static table, just look in get_funcs.
	std::pair<MetaType*,size_t> get_meta_type(const std::string& name) {return m_meta_types.Get(name);}
	MetaType& get_meta_type(const UID& uid) { return m_meta_types.GetByIndex(uid.index); }
	
private:
	IndexedHashmap<std::string, Variable> m_variables;
	IndexedHashmap<std::string, std::vector<Function>> m_functions;
	IndexedHashmap<std::string, MetaType> m_meta_types;
};