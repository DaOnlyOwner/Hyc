#include "Scopes.h"
#include <cassert>

bool Scopes::predefined_types_init = false;
std::vector<CollectionStmt> Scopes::predefined_types;
std::unordered_map<CollectionStmt*, PredefinedType> Scopes::coll_to_predef={};
DeclStmt Scopes::declTrue{ std::make_unique<BaseTypeSpec>(Token(Token::Specifier::Ident,"bool")),Token(Token::Specifier::Ident,"true") };
DeclStmt Scopes::declFalse{ std::make_unique<BaseTypeSpec>(Token(Token::Specifier::Ident,"bool")),Token(Token::Specifier::Ident,"false") };

/*
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
	if (!predefined_types_init)
	{
		predefined_types.emplace_back("int");
		predefined_types.emplace_back("float");
		predefined_types.emplace_back("double");
		predefined_types.emplace_back("quad");
		predefined_types.emplace_back("uint");
		predefined_types.emplace_back("half");
		predefined_types.emplace_back("uhalf");
		predefined_types.emplace_back("short");
		predefined_types.emplace_back("ushort");
		predefined_types.emplace_back("bool");
		predefined_types.emplace_back("char");
		predefined_types.emplace_back("uchar");
		predefined_types.emplace_back("void");

		coll_to_predef[&predefined_types[0]] = PredefinedType::Int;
		coll_to_predef[&predefined_types[1]] = PredefinedType::Float;
		coll_to_predef[&predefined_types[2]] = PredefinedType::Double;
		coll_to_predef[&predefined_types[3]] = PredefinedType::Quad;
		coll_to_predef[&predefined_types[4]] = PredefinedType::UInt;
		coll_to_predef[&predefined_types[5]] = PredefinedType::Half;
		coll_to_predef[&predefined_types[6]] = PredefinedType::UHalf;
		coll_to_predef[&predefined_types[7]] = PredefinedType::Short;
		coll_to_predef[&predefined_types[8]] = PredefinedType::UShort;
		coll_to_predef[&predefined_types[9]] = PredefinedType::Bool;
		coll_to_predef[&predefined_types[10]] = PredefinedType::Char;
		coll_to_predef[&predefined_types[11]] = PredefinedType::UChar;
		coll_to_predef[&predefined_types[12]] = PredefinedType::Void;

		predefined_types_init = true;

		declTrue.type = Type(&predefined_types[9]);
		declFalse.type = Type(&predefined_types[9]);
	}

	for (auto& p : predefined_types)
	{
		add(&p);
	}

	top_level.add(&declTrue);
	top_level.add(&declFalse);
}

//bool Scopes::add_to_existing(const std::string& name, AllocaInst* inst) { get_current_entry().table.add_to_existing(name, inst); }

// No shadowing allowed.
bool Scopes::add(DeclStmt* decl)
{
	if (!get_current_entry().table.add(decl)) return false;
	for (int64_t i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_variable(decl->name.text);
		if (elem != nullptr) return false;
	}
	return true;
}

DeclStmt* Scopes::get_variable(const std::string& name)
{
	auto tl = top_level.get_variable(name);
	if (tl) return tl;
	for (int64_t i = m_current_index; i >= 0; i = get_entry(i).father)
	{
		t_entry& e = get_entry(i);
		auto* elem = e.table.get_variable(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

//AllocaInst* Scopes::get_alloca_inst(const std::string& name)
//{
//	return get_current_entry().table.get_alloca_inst(name);
//}

void Scopes::ascend()
{
	t_entry& current = get_current_entry();
	m_current_index = current.father;
}

// Expands the tree, allocates a new node and descends to it

int64_t Scopes::expand()
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

void Scopes::descend(int64_t nthChild)
{
	// Iterate until we find the nth node that points to the current index
	int count = 0;
	for (int64_t i = m_current_index; i < m_collection.size(); i++)
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
