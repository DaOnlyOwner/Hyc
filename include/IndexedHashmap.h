#pragma once
#include <vector>
#include <unordered_map>
#include <array>
#include <type_traits>
#include <initializer_list>

template<typename TKey,typename TValue>
class Hashmap
{
public:
	Hashmap() = default;
	Hashmap(std::initializer_list<TValue> val)
		:m_container(val){}
	TValue& get(const TKey& key);
	bool insert(const TKey& key, const TValue& value);
	bool insert(TKey&& key, TValue&& value);
	size_t size();
private:
	std::unordered_map<TKey, TValue> m_container;
};

template<typename TKey, typename TValue, size_t Size>
class PerfectHashmap
{
public:
	PerfectHashmap() = default;
	PerfectHashmap(std::initializer_list<TValue> val)
		:m_container(val){}
	static_assert(std::is_base_of<size_t, TKey>::value && std::is_enum<TKey>::value,
		"Error: TKey must inherit from size_t and be an enum (each element has to be mapped to a unique index)");
	TValue& get(TKey index);
	bool insert(TKey index, const TValue& val);
	bool insert(TKey index, TValue&& val);
	size_t size();
private:
	std::array<TValue, Size> m_container;
};


// TODO: Performance: Replace with faster hashmap implementation (like robin hood hashing etc.)
template<typename TKey, typename TElem>
class IndexedHashmap
{
public:
	IndexedHashmap(size_t index_offset) 
		:m_index_offset(index_offset + 1){} // Make sure it starts at one.
	IndexedHashmap() = default;
	std::pair<TElem*,size_t> get(const TKey& key)
	{
		auto it = m_keyLookup.find(key);
		if (it == m_keyLookup.end())
		{
			return std::make_pair(nullptr,0);
		}

		else
		{
			return std::make_pair(m_elements.data() + it->second, it->second - m_elements.data() + m_index_offset - 1);
		}
	}

	// The lookup "index" is always 1 greater than the actual index ( so that 0 is a non valid index)
	TElem& get_by_index(size_t index)
	{
#if NDEBUG
		return m_elements[index - m_index_offset];
#else
		return m_elements.at(index - m_index_offset);
#endif
	}

	size_t add(const TKey& key, TElem&& elem)
	{
		unsigned int tmpSize = m_keyLookup.size();
		m_keyLookup.insert(key, m_elements.size());
		if (m_keyLookup.size() > tmpSize)
		{
			m_elements.push_back(std::forward(elem));
			return m_elements.size() + m_index_offset;
		}
		else return 0;
	}

	size_t size()
	{
		return m_elements.size();
	}

private:
	Hashmap<TKey, size_t> m_keyLookup;
	std::vector<TElem> m_elements;
	size_t m_index_offset = 0;
};
