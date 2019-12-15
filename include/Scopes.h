#pragma once

#include "SymbolTable.h"
#include <vector>
#include "Primitive.h"

/*
    r
  / .. \
 c1    cn	=> [r,c1, descendants of c1, c2, descendants of c2, ..., cn, descendants of cn]
/_\ .. /_\   

*/

class Scopes
{
public:
	

	Scopes()
	{
		m_collection.emplace_back(t_entry{ 0,SymbolTable() });
	}
	bool add(Variable&& v) { get_current_entry().table.add(std::move(v)); }
	bool add(MetaType&& mt) { get_current_entry().table.add(std::move(mt)); }
	bool add(Function&& fn) { get_current_entry().table.add(std::move(fn)); }

	std::pair<Variable*,UID> get_var(const std::string& name); 
	Variable& get_var(const UID& uid);

	std::pair<std::vector<Function>*,UID> get_funcs(const std::string& name);
	std::vector<Function>& get_funcs(const UID& uid);

	std::pair<MetaType*,UID> get_meta_type(const std::string& name);
	MetaType& get_meta_type(const UID& uid);

	void ascend();
	// Expands the tree, allocates a new node and descends to it
	size_t expand();
	

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void descend_next();
	
	void go_to_root(){ m_current_index = 0;}

	void descend(size_t nthChild);


private:
	struct t_entry
	{
		size_t father;
		SymbolTable table;
	};
	std::vector<t_entry> m_collection;
	size_t m_current_index = 0;
	t_entry& get_current_entry()
	{
		return m_collection[m_current_index];
	}
	t_entry& get_entry(size_t index)
	{
		return m_collection[index];
	}

};


