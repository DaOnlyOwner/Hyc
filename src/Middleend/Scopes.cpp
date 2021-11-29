#include "Scopes.h"
#include <cassert>
#include "Operators.h"

#define GET_ELEM_BY_NAME(symbol_table_func)\
int father = get_entry(m_current_index).father;\
for (int i = m_current_index; i >= 0; father = get_entry(m_current_index).father)\
{\
	t_entry& e = get_entry(i);\
	auto* elem = e.table.symbol_table_func(name);\
	if (elem != nullptr) return elem;\
}\
return nullptr;

/*Variable* Scopes::get_var(const std::string & name)
{
	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_var(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}


std::unordered_map<std::string,Type> Scopes::m_predefined_types 
{
	{"u8",Type("u8")},
	{"u16",Type("u16")},
	{"u32",Type("u32")},
	{"uint",Type("uint")},
	{"s8",Type("s8")},
	{"s16",Type("s16")},
	{"s32",Type("s32")},
	{"int",Type("int")},
	{"float",Type("float")},
	{"double",Type("double")}
};


Scopes::Scopes()
{
	for (int i = 0; i < (int)UnaryOp::Specifier::Count; i++)
	{
		UnaryOp::Specifier spec = (UnaryOp::Specifier)i;
		for (int j = 0; j < (int)Primitive::Specifier::Count; j++)
		{
			Primitive::Specifier pspec = (Primitive::Specifier)j;
			Type rt;
			if (spec == UnaryOp::Specifier::Minus)
				rt = j <= 3 ? get_primitive_type((Primitive::Specifier)(j + 4)) : get_primitive_type(pspec);
			else rt = get_primitive_type(pspec);
			Function* fn = new Function(UnaryOp::Translate(spec), std::vector<Type>{get_primitive_type(pspec)}, std::move(rt));
			auto& e = get_entry(0);
			e.table.add(fn);
		}
	}


	for (int i = 0; i < (int)BinOp::Specifier::Count; i++)
	{
		BinOp::Specifier spec = (BinOp::Specifier)i;
		for (int t0 = 0; t0 < (int)Primitive::Specifier::Count; t0++)
		{
			Primitive::Specifier lh = (Primitive::Specifier)t0;
			for (int t1 = 0; t1 < (int)Primitive::Specifier::Count; t1++)
			{
				Primitive::Specifier rh = (Primitive::Specifier)t1;
				auto& e = get_entry(0);
				std::vector<Type> args = { get_primitive_type(lh),get_primitive_type(rh) };
				Function* op = new Function(BinOp::Translate(spec), std::move(args), get_primitive_type(rh));
				e.table.add(op);
			}
		}
	}
}

Type* Scopes::get_type(const std::string& name)
{
	auto it = m_predefined_types.find(name);
	if (it != m_predefined_types.end()) return &it->second;


	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_type(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

std::vector<Function> Scopes::get_all_funcs(const std::string& name)
{
	std::vector<Function> out;
	int father = get_entry(m_current_index).father;
	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto elem = e.table.get_funcs(name);
		out.insert(out.begin(), elem.begin(), elem.end());
	}
	return out;
}*/

Scopes::Scopes()
{
	/*predefined_types = 
	{
		{new CollectionStmt(Token(Token::Specifier::Integer,"int"))}
	}*/
}

std::optional<std::pair<CollectionStmt*, BaseType*>> Scopes::get_type_both(const std::string & str) const
{
	auto out = m_collection[0].table.get_type(str);
	return out;
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

/*bool Scopes::is_type_defined(const Type& t)
{
	std::string base = t.get_base_type();
	return get_type(base) != nullptr;
}*/

// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
// This is really fast - just an increment operation because of the way the entries are located in the vector.

void Scopes::descend()
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

bool Scopes::go_to_father(int amount)
{
	for (int i = 0; i < amount; i++)
	{
		m_current_index = get_entry(m_current_index).father;
		if (m_current_index == -1)
		{
			m_current_index = 0;
			return false;
		}
	}
	return true;
}
