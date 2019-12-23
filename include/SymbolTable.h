#pragma once

#include <string>
#include "PerfectHashmap.h"
#include "Hashmap.h"
#include "MetaType.h"
#include "Primitive.h"
#include <algorithm>
#include <optional>
#include <memory>
#include "DebugPrint.h"

// TODO: Performance: Use move constructor for strings too.
struct Function 
{
	Function(const std::string& name, std::vector<MetaType*>&& arguments, MetaType* return_type)
		: name(name), arguments(std::move(arguments)), return_type{ return_type }{}
	Function() =default;
	std::string name;
	std::vector<MetaType*> arguments;
	MetaType* return_type = nullptr;
	bool operator==(const Function& other) { return name == other.name && arguments == other.arguments; }
	bool operator!=(const Function& other) { return !(*this == other); }
};

struct Variable
{
	Variable(const std::string& name, MetaType* type)
		:name(name), type(type){}
	std::string name;
	MetaType* type;
	bool operator==(const Variable& other) { return name == other.name && type == other.type; }
	bool operator!=(const Variable& other) { return !(*this == other); }
};

struct UnaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Count
	};

	UnaryOperator(Specifier spec, MetaType* rh):
		operation(spec),rh(rh),return_type(rh){}

	static Specifier from_token_specifier(Token::Specifier spec)
	{
		switch (spec)
		{
		
		case Token::Specifier::Plus:
			return Specifier::Plus;
		case Token::Specifier::Minus:
			return Specifier::Minus;
		default:
			Debug("bad unary specifier from token specifier");
			abort();
		}
	}

	Specifier operation;
	MetaType* rh;
	MetaType* return_type;
};

struct BinaryOperator
{
	enum class Specifier : size_t
	{
		Plus,Minus,Multiplication,Division,Count
	};

	BinaryOperator(Specifier spec, MetaType* lh, MetaType* rh):
		operation(spec),rh(rh),lh(lh),return_type(rh){}

	

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
			Debug("bad binary specifier from token specifier");
			abort();
		}
	}

	Specifier operation;
	MetaType* lh;
	MetaType* rh;
	MetaType* return_type;

};


class SymbolTable
{
public:
	SymbolTable()
		: m_binary_operators(),m_unary_operators() {}

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	SymbolTable(SymbolTable&& other) = default;
	SymbolTable& operator=(SymbolTable&& other) = default;



	bool add(Variable* var) { return m_variables.insert(std::string(var->name), std::unique_ptr<Variable>(var)); }
	bool add(Function* fn);
	bool add(MetaType* mt) { return m_meta_types.insert(std::string(mt->get_name()), std::unique_ptr<MetaType>(mt)); }

	Variable* get_var(const std::string& name)
	{
		auto* elem = m_variables.get(name);
		return elem == nullptr ? nullptr : elem->get();
	}
	MetaType* get_meta_type(const std::string& name)
	{
		auto* elem = m_meta_types.get(name);
		return elem == nullptr ? nullptr : elem->get();
	}

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{

		auto* vars = m_functions.get(name);
		if (vars != nullptr)
		{
			auto& it = std::find_if(vars->begin(), vars->end(), [](const std::unique_ptr<Function>& func) {return pred(func.get()); });
			if (it != vars->end()) return it->get();
		}
		return nullptr;
	}

	template<typename Pred>
	UnaryOperator* get_unary_operator(UnaryOperator::Specifier name, Pred pred)
	{
		auto& vars = m_unary_operators.get(name);
		auto it = std::find_if(vars.begin(), vars.end(), [pred](const std::unique_ptr<UnaryOperator>& func) {return pred(func.get()); });
		if (it != vars.end()) return it->get();
		return nullptr;
	}

	template<typename Pred>
	BinaryOperator* get_binary_operator(BinaryOperator::Specifier name, Pred pred) {

		auto& vars = m_binary_operators.get(name);
		auto it = std::find_if(vars.begin(), vars.end(), [pred](const std::unique_ptr<BinaryOperator>& func) {return pred(func.get()); });
		if (it != vars.end()) return it->get();
		return nullptr;
	}

private:
	Hashmap<std::string, std::unique_ptr<Variable>> m_variables;
	Hashmap<std::string, std::unique_ptr<MetaType>> m_meta_types;

	// binary and unary operators are statically known at compile time, so we can store them in a perfect hashmap
	PerfectHashmap<BinaryOperator::Specifier, std::vector<std::unique_ptr<BinaryOperator>>, static_cast<size_t>(BinaryOperator::Specifier::Count)> m_binary_operators;

	PerfectHashmap<UnaryOperator::Specifier, std::vector<std::unique_ptr<UnaryOperator>>, static_cast<size_t>(UnaryOperator::Specifier::Count)> m_unary_operators;

	// Functions need a hashmap to lookup the name.
	Hashmap<std::string, std::vector<std::unique_ptr<Function>>> m_functions;
};