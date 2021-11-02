#include "Scopes.h"
#include <cassert>

#define GET_ELEM_BY_NAME(symbol_table_func)\
int father = get_entry(m_current_index).father;\
for (int i = m_current_index; i >= 0; father = get_entry(m_current_index).father)\
{\
	t_entry& e = get_entry(i);\
	auto* elem = e.table.symbol_table_func(name);\
	if (elem != nullptr) return elem;\
}\
return nullptr;

Variable* Scopes::get_var(const std::string & name)
{
	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_var(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}


Type* Scopes::get_meta_type(const std::string& name)
{
	if (name[0] == 'u')
	{
		if (name[1] == '6' && name[2] == '4' && name.size() == 3) return get_primitive_type(Primitive::Specifier::u64);
		if (name[1] == '3' && name[2] == '2' && name.size() == 3) return get_primitive_type(Primitive::Specifier::u32);
		if (name[1] == '1' && name[2] == '6' && name.size() == 3) return get_primitive_type(Primitive::Specifier::u16);
		if (name[1] == '8' && name.size() == 2) return get_primitive_type(Primitive::Specifier::u8);
	}

	else if (name[0] == 's')
	{
		if (name[1] == '6' && name[2] == '4' && name.size() == 3) return get_primitive_type(Primitive::Specifier::s64);
		if (name[1] == '3' && name[2] == '2' && name.size() == 3) return get_primitive_type(Primitive::Specifier::s32);
		if (name[1] == '1' && name[2] == '6' && name.size() == 3) return get_primitive_type(Primitive::Specifier::s16);
		if (name[1] == '8' && name.size() == 2) return get_primitive_type(Primitive::Specifier::s8);
	}

	else if (name == "float") return get_primitive_type(Primitive::Specifier::Float);
	else if (name == "double") return get_primitive_type(Primitive::Specifier::Double);

	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_meta_type(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

void Scopes::ascend()
{
	t_entry& current = get_current_entry();
	m_current_index = current.father;
}

// Expands the tree, allocates a new node and descends to it

int Scopes::expand()
{
	auto indexTablePair = t_entry(m_current_index, SymbolTable());
	m_collection.push_back(std::move(indexTablePair));
	m_current_index = m_collection.size() - 1;
	return m_current_index;
}

// Expands the tree, allocates a new node and descends to it

// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
// This is really fast - just an increment operation because of the way the entries are located in the vector.

void Scopes::descend_next()
{
	if (m_current_index == m_collection.size() - 1)
	{
		go_to_root();
		return;
	}
	else ++m_current_index;
}

void Scopes::descend(size_t nthChild)
{
	// Iterate until we find the nth node that points to the current index
	int count = 0;
	for (int i = m_current_index; i < m_collection.size(); i++)
	{
		auto& e = get_entry(i);
		count += e.father == m_current_index;
		if (count == nthChild)
		{
			m_current_index = i;
			return;
		}
	}
}

void Scopes::debug_print()
{
	int tmp_current_index = m_current_index;
}
