#pragma once
#include <vector>
#include <unordered_map>
#include <array>
#include <type_traits>
#include <initializer_list>
#include <optional>

template<typename TKey,typename TValue>
class Hashmap
{
public:
	Hashmap() = default;
	Hashmap(std::unordered_map<TKey,TValue> val)
		:m_container(val){}

	Hashmap(Hashmap&&) noexcept = default;
	Hashmap(const Hashmap&) = default;
	Hashmap& operator=(Hashmap&&) = default;
	Hashmap& operator=(const Hashmap&) = default;

	TValue* get(const TKey& key)
	{

		auto it = m_container.find(key);
		if (it != m_container.end())
		{
			return &it->second;
		}

		return nullptr;
	}
	TValue* insert(const TKey& key, const TValue& value)
	{
		auto iteratorSucceed = m_container.insert(std::make_pair(key,value));
		if (iteratorSucceed.second)
		{
			return &m_container[key];
		}
		else return nullptr;
	}

	TValue* insert(TKey&& key, TValue&& value)
	{
		auto iteratorSucceed = m_container.insert(std::make_pair( std::move(key),std::move(value) ));
		if (iteratorSucceed.second)
		{
			return &iteratorSucceed.first->second;
		}
		else return nullptr;
	}
	size_t size()
	{
		return m_container.size();
	}
private:
	std::unordered_map<TKey, TValue> m_container;
};