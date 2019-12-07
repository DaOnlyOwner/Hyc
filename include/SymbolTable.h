#pragma once

#include "Symbol.h"
#include <string>
#include <map>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include "DebugPrint.h"



template<typename TKey, typename TElem>
class IndexedHashmap
{
public:
	TElem* Get(const TKey& key)
	{
		auto it = m_keyLookup.find(key);
		if (it == m_keyLookup.end())
		{
			return nullptr;
		}

		else
		{
			return m_elements.data() + it->second;
		}
	}
	
	// The lookup "index" is always 1 greater than the actual index.
	TElem& GetByIndex(size_t index)
	{
#if DEBUG
		return m_elements.at(index-1);
#else
		return m_elements[uid-1];
#endif
	}

	size_t Add(const TKey& key, TElem&& elem)
	{
		unsigned int tmpSize = m_keyLookup.size();
		m_keyLookup.insert(std::make_pair(key, m_elements.size()));
		if (m_keyLookup.size() > tmpSize)
		{
			m_elements.push_back(std::move(elem));
			return m_elements.size();
		}
		else return 0;
	}

	size_t Size()
	{
		return m_elements.size();
	}

private:
	std::unordered_map<TKey, size_t> m_keyLookup;
	std::vector<TElem> m_elements;
};


// TODO: Add methods for taking just the value
class SymbolTable
{
public:
	bool Add(Variable&& s);
	bool Add(Function&& s);
	bool Add(Struct&& s);

	// TODO: Check if key is wrong.
	template<typename TElem>
	TElem& GetByIndex(unsigned int index)
	{
		if constexpr (std::is_same<TElem, decltype(m_variables)>::value)
			return m_variables.GetByIndex(key);
		if constexpr (std::is_same < TElem, decltype(m_functions)>::value)
			return m_functions.GetByIndex(key);
		if constexpr (std::is_same < TElem, decltype(m_structs)>::value)
			return m_structs.GetByIndex(key);
	}

	template<typename TElem>
	TElem* GetByName(const std::string& name)
	{
		if constexpr (std::is_same<TElem, Variable>::value)
			return m_variables.Get(key);
		else if constexpr (std::is_same < TElem, Function>::value)
			return m_functions.Get(key);
		else if constexpr (std::is_same < TElem, Struct>::value)
			return m_structs.Get(name);
		else
		{
			static_assert(false, "Cannot get element of templated type");
		}
	}


	Type& GetByUID(const UID& uid)
	{
		if (uid.Type == UID::Struct)
			return m_structs.GetByIndex(uid.Index);
	}

private:
	IndexedHashmap<std::string, Variable> m_variables;
	IndexedHashmap<std::string, std::vector<Function>> m_functions;
	IndexedHashmap<std::string, Struct> m_structs;
};