#pragma once

#include <string>
#include "PerfectHashmap.h"
#include "AtomicType.h"
#include "Primitive.h"
#include <algorithm>
#include <optional>
#include <memory>
#include "DebugPrint.h"
#include <iterator>

namespace
{
	template<typename T,typename TValue>
	TValue* insert_into(T& insert_into, TValue* value, const std::string& key)
	{
		auto [inserted, succ] = insert_into.insert({ key,std::unique_ptr<TValue>(value) });
		if (succ)
		{
			return inserted->second.get();
		}
		return nullptr;
	}
}


// TODO: Performance: Use move constructor for strings too.
struct Function 
{
	Function(const std::string& name, std::vector<Type>&& arguments, const Type& return_type)
		: name(name), arguments(std::move(arguments)), return_type{ return_type }{}
	Function() = default;
		//:name(""),arguments(),return_type(nullptr){}
	std::string name = "";
	std::vector<Type> arguments{};
	Type return_type = error_type;
	bool operator==(const Function& other) { return name == other.name && arguments == other.arguments; }
};

extern Function error_function;

struct Variable
{
	Variable(const std::string& name, const Type& type)
		:name(name), type(type){}
	Variable() = default;
		//:name(""),type(nullptr){}
	std::string name ="";
	Type type = error_type;
	bool operator==(const Variable& other) { return name == other.name && type == other.type; }
	bool operator!=(const Variable& other) { return !(*this == other); }
};

extern Variable error_variable;

class SymbolTable
{
public:
	SymbolTable() = default;

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	SymbolTable(SymbolTable&& other) noexcept= default;
	SymbolTable& operator=(SymbolTable&& other) = default;



	Variable* add(Variable* var) { 
		return insert_into(m_variables, var, var->name);
	}
	Function* add(Function* fn);
	Type* add(Type* mt) { 
		return insert_into(m_meta_types, mt, mt->as_str()); // This is just the base case e.g. struct A -> "A" gets inserted, never a pointer type etc.
	}

	Variable* get_var(const std::string& name)
	{
		auto it = m_variables.find(name);
		if (it == m_variables.end()) return nullptr;
		return it->second.get();
	}
	Type* get_type(const std::string& name)
	{
		auto it = m_meta_types.find(name);
		if (it == m_meta_types.end()) return nullptr;
		return it->second.get();
	}

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{
		auto it = m_functions.find(name);
		if (it == m_functions.end()) return nullptr;
		auto& vars = it->second;
		auto& it2 = std::find_if(vars.begin(), vars.end(), [&](auto& func) {return pred(*func); });
		if (it2 != vars.end()) return it2->get();
		return nullptr;
	}

	std::vector<Function> get_funcs(const std::string& name)
	{
		auto it = m_functions.find(name);
		if (it == m_functions.end()) return {};
		std::vector<Function> out;
		std::transform(it->second.begin(), it->second.end(), std::back_inserter(out), [](auto& func) {return *func; });
		return out;
	}

private:
	std::unordered_map<std::string, std::unique_ptr<Variable>> m_variables;
	std::unordered_map<std::string, std::unique_ptr<Type>> m_meta_types;

	// Functions need a hashmap to lookup the name.
	std::unordered_map<std::string, std::vector<std::unique_ptr<Function>>> m_functions;
};