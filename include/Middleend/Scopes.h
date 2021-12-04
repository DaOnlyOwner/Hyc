#pragma once

#include "SymbolTable.h"
#include <vector>
#include "Primitive.h"
#include "PerfectHashmap.h"
#include <unordered_map>

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
	bool add(FuncDefStmt* fn) { return top_level.add(fn); }
	bool add(CollectionStmt* cs) {	return top_level.add(cs); }
	bool add(CollectionStmt* cs, DeclStmt* decl) { return top_level.add(cs, decl); }


	//Variable* get_var(const std::string& name);
	//Type* get_type(const std::string& name);
	bool is_type_defined(const std::string& t) const { return get_top_level_entry().table.get_type(t) != nullptr; }
	bool is_type_predefined(CollectionStmt* bt) const { return std::find_if(predefined_types.begin(), predefined_types.end(), [&](auto& p) {return p.get() == bit; }) != predefined_types.end(); }
	PredefinedType get_predefined_type(CollectionStmt* bt);


	CollectionStmt* get_type(const std::string& name) { return get_top_level_entry().table.get_type(name); }
	DeclStmt* get_decl_for(CollectionStmt* cs, const std::string& name) { return top_level.get_decl_for(cs, name); }

	static const Type& get_primitive_type(const std::string name) { return predefined_types[name]; };
	static const Type& get_primitive_type(Primitive::Specifier primitive) { return predefined_types[Primitive::Translate(primitive)]; }


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
	void descend(size_t nthChild);

	void go_to_root() { m_current_index = 0; }
	bool go_to_father(int amount);

private:
	struct t_entry
	{
		t_entry(int father, SymbolTable&& table)
			:father(father),
			table(std::move(table)) {}
		int father;
		SymbolTable table;
	};

	SymbolTable top_level;
	std::vector<t_entry> m_collection;
	int m_current_index = -1;
	int savepoint = -1;
	t_entry& get_current_entry() 
	{
		return m_collection[m_current_index];
	}

	const t_entry& get_current_entry() const
	{
		return m_collection[m_current_index];
	}

	const t_entry& get_top_level_entry() const
	{
		return m_collection[0];
	}

	t_entry& get_entry(int index)
	{
		return m_collection[index];
	}

	static std::vector<CollectionStmt> predefined_types;
	static std::vector<FuncDefStmt> predefined_funcs;

	static bool predefined_types_init;
};


