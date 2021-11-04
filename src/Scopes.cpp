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


PerfectHashmap<Primitive::Specifier, Primitive, (size_t)Primitive::Specifier::Count> Scopes::m_predefined_types 
{
	{Primitive("u8",Primitive::Specifier::u8),
	Primitive("u16",Primitive::Specifier::u16),
	Primitive("u32",Primitive::Specifier::u32),
	Primitive("uint",Primitive::Specifier::uint),
	Primitive("s8",Primitive::Specifier::s8),
	Primitive("s16",Primitive::Specifier::s16),
	Primitive("s32",Primitive::Specifier::s32),
	Primitive("int",Primitive::Specifier::int_),
	Primitive("float",Primitive::Specifier::Float),
	Primitive("double",Primitive::Specifier::Double)}
};

PerfectHashmap<UnaryOperator::Specifier, std::unordered_map<std::string, UnaryOperator>, (size_t)UnaryOperator::Specifier::Count> Scopes::m_predefined_unary_ops;
PerfectHashmap<BinaryOperator::Specifier, std::unordered_map<std::string, std::unordered_map<std::string, BinaryOperator>>, (size_t)BinaryOperator::Specifier::Count> Scopes::m_predefined_binary_ops;

void Scopes::init()
{
	// Init unary ops
	for (int i = 0; i < (int)UnaryOperator::Specifier::Count; i++)
	{
		std::unordered_map<std::string, UnaryOperator> ops;
		UnaryOperator::Specifier spec = (UnaryOperator::Specifier)i;
		for (int j = 0; j < (int)Primitive::Specifier::Count; j++)
		{
			Primitive::Specifier pspec = (Primitive::Specifier)j;
			ops[Primitive::Translate(pspec)] = UnaryOperator(spec, get_primitive_type(pspec));
		}
		m_predefined_unary_ops[spec] = ops;
	}

	// Init binary ops
	for (int i = 0; i < (int)BinaryOperator::Specifier::Count; i++)
	{
		std::unordered_map<std::string, std::unordered_map<std::string, BinaryOperator>> ops_all;
		BinaryOperator::Specifier spec = (BinaryOperator::Specifier)i;
		for (int t0 = 0; t0 < (int)Primitive::Specifier::Count; t0++)
		{
			std::unordered_map<std::string, BinaryOperator> ops_rh;
			Primitive::Specifier lh = (Primitive::Specifier)t0;
			for (int t1 = 0; t1 < (int)Primitive::Specifier::Count; t1++)
			{
				Primitive::Specifier rh = (Primitive::Specifier)t1;
				ops_rh[Primitive::Translate(rh)] = BinaryOperator(spec, get_primitive_type(lh), get_primitive_type(rh));
			}
			ops_all[Primitive::Translate(lh)] = ops_rh;
		}
		m_predefined_binary_ops[spec] = ops_all;
	}

}


Type* Scopes::get_meta_type(const std::string& name)
{
	if (name == "int") return get_primitive_type(Primitive::Specifier::int_);
	if (name == "uint") return get_primitive_type(Primitive::Specifier::uint);
	if (name[0] == 'u')
	{
		if (name[1] == '3' && name[2] == '2' && name.size() == 3) return get_primitive_type(Primitive::Specifier::u32);
		if (name[1] == '1' && name[2] == '6' && name.size() == 3) return get_primitive_type(Primitive::Specifier::u16);
		if (name[1] == '8' && name.size() == 2) return get_primitive_type(Primitive::Specifier::u8);
	}

	else if (name[0] == 's')
	{
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

UnaryOperator* Scopes::get_predef_unary_operator(UnaryOperator::Specifier name, const Type& lh)
{
	auto& vars = m_predefined_unary_ops.get(name);
	auto it = vars.find(lh.get_name());
	if (it != vars.end()) return &(*it).second;
	return nullptr;
}

BinaryOperator* Scopes::get_predef_binary_operator(BinaryOperator::Specifier name, const Type& lh, const Type& rh) {

	auto& vars = m_predefined_binary_ops.get(name);
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

UnaryOperator* Scopes::get_unary_operator(UnaryOperator::Specifier name, const Type& lh)
{
	auto* predef_op = get_predef_unary_operator(name, lh);
	if (predef_op != nullptr) return predef_op;
	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto elem = e.table.get_unary_operator(name, lh);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

BinaryOperator* Scopes::get_binary_operator(BinaryOperator::Specifier name, const Type& lh, const Type& rh)
{
	auto* predef_op = get_predef_binary_operator(name, lh, rh);
	if (predef_op != nullptr) return predef_op;
	for (int i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto elem = e.table.get_binary_operator(name, lh, rh);
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
