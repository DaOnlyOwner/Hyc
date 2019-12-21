#pragma once

#include <string>
#include "IndexedHashmap.h"
#include "MetaType.h"
#include "UID.h"
#include "Primitive.h"
#include <algorithm>

#define GET_ELEM_BY_NAME_AND_FN(container_name)\
auto* vars = container_name##.get(name);\
if (vars.get(name) != nullptr)\
{\
	auto it = std::find_if(vars->begin(), vars->end(), fn);\
	if (it != vars->end()) return *it;\
}\
return nullptr;

// TODO: Performance: Use move constructor for strings too.
struct Function 
{
	Function(const std::string& name, std::vector<UID>&& arguments)
		: name(name), arguments(std::move(arguments)){}
	std::string name;
	UID return_type;
	std::vector<UID> arguments;
	bool operator==(const Function& other) { name == other.name && arguments == other.arguments; }
	bool operator!=(const Function& other) { !(*this == other); }
};

struct Variable
{
	Variable(const std::string& name, UID type)
		:name(name), type(type){}
	std::string name;
	UID type;
	bool operator==(const Variable& other) { name == other.name && type == other.type; }
	bool operator!=(const Variable& other) { !(*this == other); }
};

struct UnaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Count
	};

	Specifier operation;
	UID rh;
};

struct BinaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Multiplication,Division,Count
	};

	Specifier operation;
	UID lh, rh;

};


class SymbolTable
{
public:
	SymbolTable() = default;
	SymbolTable(size_t offset)
		:m_meta_types(offset), m_variables(offset){}

	bool add(Variable&& var) { return m_variables.add(var.name,std::move(var)) > 0; }
	bool add(Function&& fn);
	bool add(MetaType&& mt) { return m_meta_types.add(mt.get_name(), std::move(mt)) > 0; }

	std::pair<Variable*, size_t> get_var(const std::string& name) { auto ret = m_variables.get(name); return std::make_pair(ret.first, ret.second); }
	Variable& get_var(const UID& uid) { return m_variables.get_by_index(uid.index); }

	std::pair<MetaType*,size_t> get_meta_type(const std::string& name) {return m_meta_types.get(name);}
	MetaType& get_meta_type(const UID& uid) { return m_meta_types.get_by_index(uid.index); }

	// Returns the function for which fn returns true.
	template<typename Fn>
	Function* get_func(const std::string& name, Fn fn) 
	{
		GET_ELEM_BY_NAME_AND_FN(m_functions)
	}
	
	template<typename Fn>
	UnaryOperator* get_unary_operator(UnaryOperator::Specifier name) 
	{
		GET_ELEM_BY_NAME_AND_FN(m_unary_operators)
	}

	BinaryOperator* get_binary_operator(BinaryOperator::Specifier name) {
		GET_ELEM_BY_NAME_AND_FN(m_binary_operators)
	}
	
private:
	IndexedHashmap<std::string, Variable> m_variables;
	IndexedHashmap<std::string, MetaType> m_meta_types;
	
	// We don't need indices here because functions / operators cannot be "carried around" in code.
	// binary and unary operators are statically known at compile time, so we can store them in a perfect hashmap
	PerfectHashmap<BinaryOperator::Specifier, std::vector<BinaryOperator>,static_cast<unsigned int>(BinaryOperator::Specifier::Count)> m_binary_operators;
	PerfectHashmap<UnaryOperator::Specifier, std::vector<UnaryOperator>, static_cast<unsigned int>(UnaryOperator::Specifier::Count)> m_unary_operators;

	// Functions need a hashmap to lookup the name.
	Hashmap<std::string, std::vector<Function>> m_functions;
};