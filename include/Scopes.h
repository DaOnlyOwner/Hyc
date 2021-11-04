#pragma once

#include "SymbolTable.h"
#include <vector>
#include "Primitive.h"
#include "PerfectHashmap.h"
#include <unordered_map>

#define GET_ELEM_BY_NAME_AND_PRED_SCOPES(symbol_table_func)\
size_t father = get_entry(m_current_index).father;\
for (int i = m_current_index; i >= 0; father = get_entry(m_current_index).father)\
{\
	t_entry& e = get_entry(i);\
	auto elem = e.table.##symbol_table_func##(name, pred);\
	if (elem != nullptr) return elem;\
}\
return nullptr;


/*
	r
  / .. \
 c1    cn	=> [r,c1, descendants of c1, c2, descendants of c2, ..., cn, descendants of cn]
/_\ .. /_\

*/

class Scopes
{
public:
	Scopes() = default;
	Scopes(const Scopes&) = delete;
	Scopes& operator=(const Scopes&) = delete;

	Scopes(Scopes&&) = default;
	Scopes& operator=(Scopes&&) noexcept = default;


	Variable* add(Variable&& v) { return get_current_entry().table.add(std::move(v)); }
	Type* add(Type&& mt) { return get_current_entry().table.add(std::move(mt)); }
	Function* add(Function&& fn) { return get_current_entry().table.add(std::move(fn)); }

	Variable* get_var(const std::string& name);
	Type* get_meta_type(const std::string& name);

	static Primitive* get_primitive_type(Primitive::Specifier name) { return &m_predefined_types.get(name); }

	UnaryOperator* get_predef_unary_operator(UnaryOperator::Specifier name, const Type& lh);

	BinaryOperator* get_predef_binary_operator(BinaryOperator::Specifier name, const Type& lh, const Type& rh);

	UnaryOperator* get_unary_operator(UnaryOperator::Specifier name, const Type& lh);

	BinaryOperator* get_binary_operator(BinaryOperator::Specifier name, const Type& lh, const Type& rh);

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{
		int father = get_entry(m_current_index).father;
		for (int i = m_current_index; i >= 0; i = get_entry(i).father)
		{
			t_entry& e = get_entry(i);
			auto elem = e.table.get_func(name, pred);
			if (elem != nullptr) return elem;
		}
		return nullptr;
	}

	std::vector<Function*> get_all_funcs(const std::string& name)
	{
		std::vector<Function*> out;
		int father = get_entry(m_current_index).father;
		for (int i = m_current_index; i >= 0; i = get_entry(i).father)
		{
			t_entry& e = get_entry(i);
			auto elem = e.table.get_func(name, [](auto& func) {return true; });
			if (elem != nullptr) return elem;
		}
		return nullptr;
	}


	void ascend();
	// Expands the tree, allocates a new node and descends to it
	int expand();

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void descend_next();

	void go_to_root() { m_current_index = 0; }

	void descend(size_t nthChild);

	void debug_print();

	static void init();

private:
	struct t_entry
	{
		t_entry(int father, SymbolTable&& table)
			:father(father),
			table(std::move(table)) {}
		int father;
		SymbolTable table;
	};

	std::vector<t_entry> m_collection;
	int m_current_index = -1;
	t_entry& get_current_entry()
	{
		return m_collection[m_current_index];
	}
	t_entry& get_entry(int index)
	{
		return m_collection[index];
	}


	static PerfectHashmap<Primitive::Specifier, Primitive, (size_t)Primitive::Specifier::Count> m_predefined_types;

	static PerfectHashmap<UnaryOperator::Specifier, std::unordered_map<std::string, UnaryOperator>, (size_t)UnaryOperator::Specifier::Count> m_predefined_unary_ops;
	static PerfectHashmap<BinaryOperator::Specifier, std::unordered_map<std::string, std::unordered_map<std::string, BinaryOperator>>, (size_t)BinaryOperator::Specifier::Count> m_predefined_binary_ops;
};


