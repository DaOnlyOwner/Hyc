#pragma once

#include "SymbolTable.h"
#include <vector>
#include <variant>
#include <cassert>

/*
    r
  / .. \
 c1    cn	=> [r,c1, descendants of c1, c2, descendants of c2, ..., cn, descendants of cn]
/_\ .. /_\   

*/

class StackCollection
{
public:
	// Ascends to the father node.
	StackCollection();
	bool Add(Struct&& s);
	bool Add(Variable&& v);
	bool Add(Function&& f);

	void Ascend()
	{
		t_entry& current = get_current_entry();
		assert(current.Father != 0);
		m_current_index = current.Father;
	}
	
	// Expands the tree, allocates a new node and descends to it
	size_t Expand()
	{
		m_collection.push_back({ m_current_index,SymbolTable() });
		m_current_index = m_collection.size() - 1;
		return m_current_index;
	}

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void DescendNext()
	{
		if (m_current_index == m_collection.size() - 1)
		{
			GoToRoot();
			return;
		}
		else ++m_current_index;
	}
	
	void GoToRoot()
	{
		m_current_index = 0;
	}

	void Descend(size_t nthChild)
	{
		// Iterate until we find the nth node that points to the current index
		int count = 0;
		for (size_t i = m_current_index; i < m_collection.size(); i++)
		{
			auto& e = get_entry(i);
			count += e.Father == m_current_index;
			if (count == nthChild)
			{
				m_current_index = i;
				return;
			}
		}
	}

	template<typename TElem>
	TElem& GetByIndex(UID uid)
	{
#if DEBUG
		return m_collection.at(uid.StackIndex).Table.GetByIndex<TElem>(uid.Index);
#else
		return m_collection[uid.StackIndex].Table.GetByIndex(uid.Index);
#endif
	}

	template<typename TElem>
	const TElem& GetByIndex(UID uid) const
	{
		return GetByIndex(uid);
	}


	Type& GetByUID(const UID& uid)
	{
#if DEBUG
		return m_collection.at(uid.StackIndex).Table.GetByUID(uid);
#else
		return m_collection[uid.StackIndex].Table.GetByUID(uid);
#endif
	}

	const Type& GetByUID(const UID& uid) const
	{
		return GetByUID(uid);
	}


	template<typename TElem>
	TElem* GetByName(const std::string& name)
	{
		size_t father = get_entry(m_current_index).Father;
		for (int i = m_current_index; i >= 0; father = get_entry(m_current_index).Father)
		{
			t_entry& e = get_entry(i);
			TElem* elem = e.Table.GetByName<TElem>(name);
			if (elem != nullptr) return elem;
		}
		return nullptr;
	}

	template<typename TElem>
	const TElem* GetByName(const std::string& name) const
	{
		return GetByName(name);
	}

	std::vector<Function>* GetFunctions(const std::string& name)
	{
		return GetByName<std::vector<Function>>(name);
	}

	const std::vector<Function>* GetFunctions(const std::string& name) const
	{
		return GetFunctions(name);
	}
private:
	struct t_entry
	{
		size_t Father;
		SymbolTable Table;
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


