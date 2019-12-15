#pragma once
#include <vector>
#include <unordered_map>

// TODO: Performance: Replace with faster hashmap implementation (like robin hood hashing etc.)
template<typename TKey, typename TElem>
class IndexedHashmap
{
public:
	IndexedHashmap(size_t index_offset) 
		:m_index_offset(index_offset){}
	IndexedHashmap() = default;
	std::pair<TElem*,size_t> Get(const TKey& key)
	{
		auto it = m_keyLookup.find(key);
		if (it == m_keyLookup.end())
		{
			return std::make_pair(nullptr,0);
		}

		else
		{
			return std::make_pair(m_elements.data() + it->second, it->second - m_elements.data() + m_index_offset + 1);
		}
	}

	// The lookup "index" is always 1 greater than the actual index ( so that 0 is a non valid index)
	TElem& GetByIndex(size_t index)
	{
#if NDEBUG
		return m_elements[index - m_index_offset - 1];
#else
		return m_elements.at(index - m_index_offset - 1);
#endif
	}

	size_t Add(const TKey& key, TElem&& elem)
	{
		unsigned int tmpSize = m_keyLookup.size();
		m_keyLookup.insert(std::make_pair(key, m_elements.size()));
		if (m_keyLookup.size() > tmpSize)
		{
			m_elements.push_back(std::forward(elem));
			return m_elements.size() + m_index_offset;
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
	size_t m_index_offset = 0;
};
