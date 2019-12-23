#pragma once

#include "SymbolTable.h"
#include <vector>
#include "Primitive.h"
#include "PerfectHashmap.h"

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
	Scopes() = default;
	Scopes(const Scopes&) = delete;
	Scopes& operator=(const Scopes&) = delete;

	Scopes(Scopes&&) = default;
	Scopes& operator=(Scopes&&) noexcept = default;


	bool add(Variable* v) { return get_current_entry().table.add(v); }
	bool add(MetaType* mt) { return get_current_entry().table.add(mt); }
	bool add(Function* fn) { return get_current_entry().table.add(fn); }

	Variable* get_var(const std::string& name);
	MetaType* get_meta_type(const std::string& name);

	Primitive* get_primitive_type(Primitive::Specifier name) { return &m_predefined_types.get(name); }

	template<typename Pred>
	UnaryOperator* get_unary_operator(UnaryOperator::Specifier name, Pred pred)
	{
		auto& uop = m_predefined_unary_ops.get(name);
		if (pred(&uop)) return &uop;
		int father = get_entry(m_current_index).father;
		for (int i = m_current_index; i >= 0; i = get_entry(m_current_index).father)
		{
			t_entry& e = get_entry(i);
			auto elem = e.table.get_unary_operator(name, pred);
			if (elem != nullptr) return elem;
		}
		return nullptr;

	}
	template<typename Pred>
	BinaryOperator* get_binary_operator(BinaryOperator::Specifier name, Pred pred)
	{
		auto& bop = m_predefined_binary_ops.get(name);
		if (pred(&bop)) return &bop;
		int father = get_entry(m_current_index).father;
		for (int i = m_current_index; i >= 0; i = get_entry(m_current_index).father)
		{
			t_entry& e = get_entry(i);
			auto elem = e.table.get_binary_operator(name, pred);
			if (elem != nullptr) return elem;
		}
		return nullptr;

	}

	template<typename Pred>
	Function* get_func(const std::string& name, Pred pred)
	{
		int father = get_entry(m_current_index).father;
		for (int i = m_current_index; i >= 0; i = get_entry(m_current_index).father)
		{
			t_entry& e = get_entry(i);
			auto elem = e.table.get_func(name, pred);
			if (elem != nullptr) return elem;
		}
		return nullptr;

	}


	void ascend();
	// Expands the tree, allocates a new node and descends to it
	int expand();

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void descend_next();

	void go_to_root() { m_current_index = 0; }

	void descend(size_t nthChild);


private:
	struct t_entry
	{
		t_entry(int father, SymbolTable&& table)
			:father(father),
			table(std::move(table)){}
		int father;
		SymbolTable table;
	};

	std::vector<t_entry> m_collection;
	int m_current_index = -1;
	t_entry& get_current_entry()
	{
		return m_collection[m_current_index];
	}
	t_entry& get_entry(int index)
	{
		return m_collection[index];
	}
	

	PerfectHashmap<Primitive::Specifier, Primitive, (size_t)Primitive::Specifier::Count> m_predefined_types
	{
		{Primitive("u8",Primitive::Specifier::u8),
		Primitive("u16",Primitive::Specifier::u16),
		Primitive("u32",Primitive::Specifier::u32),
		Primitive("u64",Primitive::Specifier::u64),
		Primitive("s8",Primitive::Specifier::s8),
		Primitive("s16",Primitive::Specifier::s16),
		Primitive("s32",Primitive::Specifier::s32),
		Primitive("s64",Primitive::Specifier::s64),
		Primitive("float",Primitive::Specifier::Float),
		Primitive("double",Primitive::Specifier::Double)}
	};
	
	PerfectHashmap<UnaryOperator::Specifier, UnaryOperator, (size_t)UnaryOperator::Specifier::Count * (size_t)Primitive::Specifier::Count> m_predefined_unary_ops
	{
		{
			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u8)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u8)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u16)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u16)),
			
			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u32)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u32)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u64)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u64)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s8)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s8)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s16)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s16)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s32)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s32)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s64)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s64)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::Float)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::Float)),

			UnaryOperator(UnaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::Double)),
			UnaryOperator(UnaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::Double))
		}
	};

	PerfectHashmap<BinaryOperator::Specifier, BinaryOperator, (size_t)BinaryOperator::Specifier::Count * (size_t)Primitive::Specifier::Count> m_predefined_binary_ops
	{
		{
			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u8),&m_predefined_types.get(Primitive::Specifier::u8)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u8),&m_predefined_types.get(Primitive::Specifier::u8)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::u8),&m_predefined_types.get(Primitive::Specifier::u8)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::u8),&m_predefined_types.get(Primitive::Specifier::u8)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u16),&m_predefined_types.get(Primitive::Specifier::u16)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u16),&m_predefined_types.get(Primitive::Specifier::u16)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::u16),&m_predefined_types.get(Primitive::Specifier::u16)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::u16),&m_predefined_types.get(Primitive::Specifier::u16)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u32),&m_predefined_types.get(Primitive::Specifier::u32)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u32),&m_predefined_types.get(Primitive::Specifier::u32)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::u32),&m_predefined_types.get(Primitive::Specifier::u32)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::u32),&m_predefined_types.get(Primitive::Specifier::u32)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::u64),&m_predefined_types.get(Primitive::Specifier::u64)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::u64),&m_predefined_types.get(Primitive::Specifier::u64)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::u64),&m_predefined_types.get(Primitive::Specifier::u64)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::u64),&m_predefined_types.get(Primitive::Specifier::u64)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s8),&m_predefined_types.get(Primitive::Specifier::s8)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s8),&m_predefined_types.get(Primitive::Specifier::s8)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::s8),&m_predefined_types.get(Primitive::Specifier::s8)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::s8),&m_predefined_types.get(Primitive::Specifier::s8)),
		
			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s16),&m_predefined_types.get(Primitive::Specifier::s16)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s16),&m_predefined_types.get(Primitive::Specifier::s16)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::s16),&m_predefined_types.get(Primitive::Specifier::s16)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::s16),&m_predefined_types.get(Primitive::Specifier::s16)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s32),&m_predefined_types.get(Primitive::Specifier::s32)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s32),&m_predefined_types.get(Primitive::Specifier::s32)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::s32),&m_predefined_types.get(Primitive::Specifier::s32)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::s32),&m_predefined_types.get(Primitive::Specifier::s32)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::s64),&m_predefined_types.get(Primitive::Specifier::s64)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::s64),&m_predefined_types.get(Primitive::Specifier::s64)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::s64),&m_predefined_types.get(Primitive::Specifier::s64)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::s64),&m_predefined_types.get(Primitive::Specifier::s64)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::Float),&m_predefined_types.get(Primitive::Specifier::Float)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::Float),&m_predefined_types.get(Primitive::Specifier::Float)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::Float),&m_predefined_types.get(Primitive::Specifier::Float)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::Float),&m_predefined_types.get(Primitive::Specifier::Float)),

			BinaryOperator(BinaryOperator::Specifier::Minus,&m_predefined_types.get(Primitive::Specifier::Double),&m_predefined_types.get(Primitive::Specifier::Double)),
			BinaryOperator(BinaryOperator::Specifier::Plus,&m_predefined_types.get(Primitive::Specifier::Double),&m_predefined_types.get(Primitive::Specifier::Double)),
			BinaryOperator(BinaryOperator::Specifier::Division,&m_predefined_types.get(Primitive::Specifier::Double),&m_predefined_types.get(Primitive::Specifier::Double)),
			BinaryOperator(BinaryOperator::Specifier::Multiplication,&m_predefined_types.get(Primitive::Specifier::Double),&m_predefined_types.get(Primitive::Specifier::Double))
		
		}
	};
};


