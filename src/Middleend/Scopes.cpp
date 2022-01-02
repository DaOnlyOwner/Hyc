#include "Scopes.h"
#include <cassert>

bool Scopes::predefined_types_init = false;
std::vector<TypeDefStmt> Scopes::predefined_types;
std::unordered_map<TypeDefStmt*, PredefinedType> Scopes::coll_to_predef={};
DeclStmt Scopes::declTrue{ std::make_unique<BaseTypeSpec>(Token(Token::Specifier::Ident,"bool")),Token(Token::Specifier::Ident,"true") };
DeclStmt Scopes::declFalse{ std::make_unique<BaseTypeSpec>(Token(Token::Specifier::Ident,"bool")),Token(Token::Specifier::Ident,"false") };

Scopes::Scopes()
	:top_level(nullptr),current_entry(&top_level)
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

	top_level.table.add(&declTrue);
	top_level.table.add(&declFalse);
	top_level.table.add(error_type.get_base_type());
}

bool Scopes::add(DeclStmt* decl)
{
	return current_entry->table.add(decl);
}

DeclStmt* Scopes::get_variable(const std::string& name)
{
	//auto tl = top_level.table.get_variable(name);
	//if (tl) return tl;
	for (entry* tbl = current_entry; tbl != nullptr; tbl = tbl->father)
	{
		auto* elem = tbl->table.get_variable(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

std::vector<std::pair<size_t, DeclStmt*>> Scopes::get_all_variables_reversed()
{
	assert(current_entry != nullptr);
	return current_entry->table.get_all_variables_reversed();
}

llvm::Value* Scopes::get_value(const std::string& name)
{
	auto tl = top_level.table.get_value(name);
	if (tl) return tl;
	for (entry* tbl = current_entry; tbl != nullptr; tbl = tbl->father)
	{
		auto* elem = tbl->table.get_value(name);
		if (elem != nullptr) return elem;
	}
	return nullptr;
}

void Scopes::ascend()
{
	current_entry = current_entry->father;
}

// Expands the tree, allocates a new node and descends to it

void Scopes::expand()
{
	std::unique_ptr<entry> e = std::make_unique<entry>(current_entry);
	current_entry->children.push_back(std::move(e));
	current_entry = current_entry->children.back().get();
}

void Scopes::descend()
{
	int64_t& child_idx = current_entry->current_child;
	int64_t child_size = current_entry->children.size();
	assert(child_size > 0);
	current_entry = current_entry->children[child_idx].get();
	child_idx++;
	if (child_idx >= child_size)
	{
		child_idx = 0;
	}
}


