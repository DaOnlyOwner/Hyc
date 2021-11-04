#pragma once

#include <string>
#include "PerfectHashmap.h"
#include "Hashmap.h"
#include "Type.h"
#include "Primitive.h"
#include <algorithm>
#include <optional>
#include <memory>
#include "DebugPrint.h"

// TODO: Performance: Use move constructor for strings too.
struct Function 
{
	Function(const std::string& name, std::vector<Type*>&& arguments, Type* return_type)
		: name(name), arguments(std::move(arguments)), return_type{ return_type }{}
	Function() = default;
		//:name(""),arguments(),return_type(nullptr){}
	std::string name = "";
	std::vector<Type*> arguments{};
	Type* return_type = &error_type;
	bool operator==(const Function& other) { return name == other.name && arguments == other.arguments; }
};

extern Function error_function;

struct Variable
{
	Variable(const std::string& name, Type* type)
		:name(name), type(type){}
	Variable() = default;
		//:name(""),type(nullptr){}
	std::string name ="";
	Type* type = &error_type;
	bool operator==(const Variable& other) { return name == other.name && type == other.type; }
	bool operator!=(const Variable& other) { return !(*this == other); }
};

extern Variable error_variable;

struct UnaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Count
	};

	UnaryOperator(Specifier spec, Type* rh):
		operation(spec),rh(rh),return_type(rh){}

	UnaryOperator() = default;
		//:operation(Specifier::Count), rh(nullptr), return_type(nullptr) {}

	static Specifier from_token_specifier(Token::Specifier spec)
	{
		switch (spec)
		{
		case Token::Specifier::Plus:
			return Specifier::Plus;
		case Token::Specifier::Minus:
			return Specifier::Minus;
		default:
			Error::Debug("bad unary specifier from token specifier");
			abort();
		}
	}

	Specifier operation = Specifier::Count;
	Type* rh = nullptr;
	Type* return_type = nullptr;
};

struct BinaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Multiplication,Division,Count
	};

	BinaryOperator(Specifier spec, Type* lh, Type* rh):
		operation(spec),rh(rh),lh(lh),return_type(rh){}
	
	BinaryOperator()
		:operation(Specifier::Count), rh(nullptr), lh(nullptr), return_type(nullptr) {};

	static Specifier from_token_specifier(Token::Specifier spec)
	{
		switch (spec)
		{
		
		case Token::Specifier::Slash:
			return Specifier::Division;
		case Token::Specifier::Plus:
			return Specifier::Plus;
		case Token::Specifier::Minus:
			return Specifier::Minus;
		case Token::Specifier::Asterix:
			return Specifier::Multiplication;
		default:
			Error::Debug("bad binary specifier from token specifier");
			abort();
		}
	}

	Specifier operation;
	Type* lh;
	Type* rh;
	Type* return_type;

};


class SymbolTable
{
public:
	SymbolTable()
		: m_binary_operators(),m_unary_operators() {}

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	SymbolTable(SymbolTable&& other) noexcept= default;
	SymbolTable& operator=(SymbolTable&& other) = default;



	Variable* add(Variable&& var) { return m_variables.insert(var.name, std::move(var)); }
	Function* add(Function&& fn);
	Type* add(Type&& mt) { return m_meta_types.insert(mt.get_name(), std::move(mt)); }

	Variable* get_var(const std::string& name)
	{
		auto* elem = m_variables.get(name);
		return elem == nullptr ? nullptr : elem;
	}
	Type* get_meta_type(const std::string& name)
	{
		auto* elem = m_meta_types.get(name);
		return elem == nullptr ? nullptr : elem;
	}

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{

		auto* vars = m_functions.get(name);
		if (vars != nullptr)
		{
			auto& it = std::find_if(vars->begin(), vars->end(), [pred](const Function& func) {return pred(func); });
			if (it != vars->end()) return &*it;
		}
		return nullptr;
	}

	std::vector<Function*> get_funcs(const std::string& name)
	{
		auto* vars = m_functions.get(name);
		if (vars == nullptr) return {};
		// TODO: Continue
		error;
	}

	UnaryOperator* get_unary_operator(UnaryOperator::Specifier name, const Type& lh)
	{
		auto& vars = m_unary_operators.get(name);
		auto it = vars.find(lh.get_name());
		if (it != vars.end()) return &(*it).second;
		return nullptr;
	}

	BinaryOperator* get_binary_operator(BinaryOperator::Specifier name, const Type& lh, const Type& rh) {

		auto& vars = m_binary_operators.get(name);
		auto it_rh = vars.find(lh.get_name());
		if (it_rh != vars.end())
		{
			auto& rh_table = (*it_rh).second;
			auto it_lh = rh_table.find(rh.get_name());
			if (it_lh != rh_table.end())
			{
				return &(*it_lh).second;
			}
		}
		return nullptr;
	}

private:
	Hashmap<std::string, Variable> m_variables;
	Hashmap<std::string, Type> m_meta_types;

	// binary and unary operators are statically known at compile time, so we can store them in a perfect hashmap
	PerfectHashmap<BinaryOperator::Specifier, std::unordered_map<std::string,std::unordered_map<std::string,BinaryOperator>>, static_cast<size_t>(BinaryOperator::Specifier::Count)> m_binary_operators;
	PerfectHashmap<UnaryOperator::Specifier, std::unordered_map<std::string,UnaryOperator>, static_cast<size_t>(UnaryOperator::Specifier::Count)> m_unary_operators;

	// Functions need a hashmap to lookup the name.
	Hashmap<std::string, std::vector<Function>> m_functions;
};