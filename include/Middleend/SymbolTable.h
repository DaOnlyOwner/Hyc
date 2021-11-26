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

// TODO: Performance: Use move constructor for strings too.
/*struct Function
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

struct AttributeCollection // Structs, unions
{
	AttributeCollection(const std::string& name)
		:name(name) {}
	std::string name;
	Stmt* def_stmt = nullptr;
};

struct Namespace
{
	Namespace(const std::string& name)
		:name(name) {}
	std::string name;
	Namespace* inner = nullptr;
	Stmt* def_stmt = nullptr;
	void set_inner_ns(Namespace* inner) { this->inner = inner; }
};


extern Variable error_variable;*/

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

	bool add(CollectionStmt* cs)
	{
		CollectionInfo info;
		info.bt = std::unique_ptr<BaseType>(new BaseType{ cs->name.text });
		info.stmt = cs;
		return collections.insert({ cs->name.text,std::move(info) }).second;
	}

	bool add(FuncDefStmt* fn);

	std::optional<std::pair<CollectionStmt*,BaseType*>> get_type(const std::string& name)
	{
		auto it = collections.find(name);
		if (it != collections.end()) return { { it->second.stmt,it->second.bt.get()} };
		return std::nullopt;
	}

	/*Type* get_type(const std::string& name)
	{
		auto it = types.find(name);
		if (it == types.end()) return nullptr;
		return it->second.get();
	}

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{
		auto it = functions.find(name);
		if (it == functions.end()) return nullptr;
		auto& vars = it->second;
		auto& it2 = std::find_if(vars.begin(), vars.end(), [&](auto& func) {return pred(*func); });
		if (it2 != vars.end()) return it2->get();
		return nullptr;
	}

	std::vector<Function> get_funcs(const std::string& name)
	{
		auto it = functions.find(name);
		if (it == functions.end()) return {};
		std::vector<Function> out;
		std::transform(it->second.begin(), it->second.end(), std::back_inserter(out), [](auto& func) {return *func; });
		return out;
	}*/

private:
	struct CollectionInfo
	{
		CollectionStmt* stmt;
		std::unique_ptr<BaseType> bt;
	};
	//std::unordered_map<std::string, std::unique_ptr<Variable>> variables;
	//std::unordered_map<std::string, std::unique_ptr<Type>> types;
	std::unordered_map<std::string, CollectionInfo> collections;
	// name maps to overloads
	std::unordered_map<std::string, std::vector<FuncDefStmt*>> functions;
};