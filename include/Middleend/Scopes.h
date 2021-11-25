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
	Scopes();
	Scopes(const Scopes&) = delete;
	Scopes& operator=(const Scopes&) = delete;

	Scopes(Scopes&&) = default;
	Scopes& operator=(Scopes&&) noexcept = default;


	//Variable* add(Variable* v) { return get_current_entry().table.add(v); }
	//Type* add(Type* mt) { return get_current_entry().table.add(mt); }
	bool add(FuncDefStmt* fn) { return get_current_entry().table.add(fn); }
	bool add(CollectionStmt* cs) { 
		// The current entry should always be the top level
		return get_current_entry().table.add(cs); }


	//Variable* get_var(const std::string& name);
	//Type* get_type(const std::string& name);
	bool is_type_defined(const std::string& t);

	CollectionStmt* get_type(const std::string& str);
	BaseType* get_base_type(const std::string& str);

	static const Type& get_primitive_type(const std::string name) { return m_predefined_types[name]; };
	static const Type& get_primitive_type(Primitive::Specifier primitive) { return m_predefined_types[Primitive::Translate(primitive)]; }


	/*template<typename Pred>
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

	std::vector<Function> get_all_funcs(const std::string& name);*/


	void ascend();
	// Expands the tree, allocates a new node and descends to it
	int expand();

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void descend();

	void go_to_root() { m_current_index = 0; }
	void go_to_father(int amount)
	{
		for (int i = 0; i < amount; i++)
		{
			m_current_index = get_entry(m_current_index).father;
			if (m_current_index == -1)
			{
				m_current_index = 0;
				return;
			}
		}
	}

	Scopes& at_root() { save(); go_to_root(); return *this; }
	Scopes& at_father(int i = 1) { save(); go_to_father(i); return *this; }
	void ret() { m_current_index = savepoint; }
	void save() { savepoint = m_current_index; }

	void descend(size_t nthChild);

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
	int savepoint = -1;
	t_entry& get_current_entry()
	{
		return m_collection[m_current_index];
	}
	t_entry& get_entry(int index)
	{
		return m_collection[index];
	}


	static std::unordered_map<std::string, Type> m_predefined_types;
};


