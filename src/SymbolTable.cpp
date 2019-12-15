#include "SymbolTable.h"

inline SymbolTable::SymbolTable()
{
	m_meta_types.Add("u8", Primitive(Primitive::Specifier::u8));
	m_meta_types.Add("u16", Primitive(Primitive::Specifier::u16));
	m_meta_types.Add("u32", Primitive(Primitive::Specifier::u32));
	m_meta_types.Add("u64", Primitive(Primitive::Specifier::u64));
	m_meta_types.Add("s8", Primitive(Primitive::Specifier::s8));
	m_meta_types.Add("s16", Primitive(Primitive::Specifier::s16));
	m_meta_types.Add("s32", Primitive(Primitive::Specifier::s32));
	m_meta_types.Add("s64", Primitive(Primitive::Specifier::s64));
	m_meta_types.Add("float", Primitive(Primitive::Specifier::Float));
	m_meta_types.Add("double", Primitive(Primitive::Specifier::Double));
}

bool SymbolTable::add(Function&& fn)
{
	auto fns = m_functions.Get(fn.name);
	if (fns.first == nullptr) m_functions.Add(fn.name, { std::move(fn) });
	// Might be an overload
	else
	{
		if (std::find(fns.first->begin(), fns.first->end(), fn) == fns.first->end())
		{
			fns.first->push_back(std::move(fn));
		}
		else return false;
	}
	return true;
}
