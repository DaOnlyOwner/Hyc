#include "Scopes.h"
#include <cassert>

#define GET_ELEM_BY_NAME(method_name)\
size_t father = get_entry(m_current_index).father;\
for (int i = m_current_index; i >= 0; father = get_entry(m_current_index).father)\
{\
	t_entry& e = get_entry(i);\
	auto elem = e.table.##method_name##(name);\
	if (elem.first != nullptr) return std::make_pair(elem.first,UID(i,elem.second));\
}\
return std::make_pair(nullptr,UID::Zero);

std::pair<Variable*, UID> Scopes::get_var(const std::string& name)
{
	GET_ELEM_BY_NAME(get_var)
}

Variable& Scopes::get_var(const UID& uid)
{
#if NDEBUG
	return m_collection[uid.stack_index].table.get_var(uid);
#else
	return m_collection.at(uid.stack_index).table.get_var(uid);
#endif
}

std::pair<std::vector<Function>*, UID> Scopes::get_funcs(const std::string& name)
{
	GET_ELEM_BY_NAME(get_funcs)
}

std::vector<Function>& Scopes::get_funcs(const UID& uid)
{
#if NDEBUG
	return m_collection[uid.stack_index].table.get_funcs(uid);
#else
	return m_collection.at(uid.stack_index).table.get_funcs(uid);
#endif
}

std::pair<MetaType*, UID> Scopes::get_meta_type(const std::string& name)
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

	GET_ELEM_BY_NAME(get_meta_type)
}

MetaType& Scopes::get_meta_type(const UID& uid)
{
	if (uid.stack_index == 0 && uid.index <= 10) return *get_primitive_type(static_cast<Primitive::Specifier>(uid.index)).first;
#if NDEBUG
	return m_collection[uid.stack_index].table.get_meta_type(uid);
#else
	return m_collection.at(uid.stack_index).table.get_meta_type(uid);
#endif
}

std::pair<Primitive*, UID> Scopes::get_primitive_type(Primitive::Specifier specifier)
{
	return { &m_primitive_lookup_table[static_cast<unsigned int>(specifier)], UID(0,static_cast<unsigned int>(specifier)) };
}

void Scopes::ascend()
{
	t_entry& current = get_current_entry();
	assert(current.father != 0);
	m_current_index = current.father;
}

// Expands the tree, allocates a new node and descends to it

size_t Scopes::expand()
{
	m_collection.push_back({ m_current_index,SymbolTable() });
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
	for (size_t i = m_current_index; i < m_collection.size(); i++)
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
