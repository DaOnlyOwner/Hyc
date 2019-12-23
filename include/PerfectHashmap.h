#pragma once
#include <vector>
#include <unordered_map>
#include <array>
#include <type_traits>
#include <initializer_list>
#include <optional>

template<typename TKey, typename TValue, size_t Size>
class PerfectHashmap
{
public:
	PerfectHashmap(std::array<TValue,Size> val)
		:m_container(val){}
	PerfectHashmap() = default;

	PerfectHashmap(PerfectHashmap&&) = default;
	PerfectHashmap(const PerfectHashmap&) = default;
	PerfectHashmap& operator=(PerfectHashmap&&) = default;
	PerfectHashmap& operator=(const PerfectHashmap&) = default;

	static_assert(std::is_enum<TKey>::value,
		"Error: TKey must be an enum that can be converted to size_t (each element has to be mapped to a unique index)");
	TValue& get(TKey index)
	{
		return m_container[static_cast<size_t>(index)];
	}

	bool insert(TKey index, const TValue& val)
	{
		m_container[static_cast<size_t>(index)] = val;
		return true;
	}

	bool insert(TKey index, TValue&& val)
	{
		m_container[static_cast<size_t>(index)] = std::move(val);
		return true;
	}
	size_t size()
	{
		return Size;
	}
private:
	std::array<TValue, Size> m_container;
};

