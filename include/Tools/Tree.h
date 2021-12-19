#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cassert>


template<typename T>
class Tree
{
	template<typename T>
	class TreeNode
	{
	public:
		TreeNode(const T& t) : data(t){}

		bool creates_circle(const T& t)
		{
			if (contains(t)) return true;
			for (auto& f : fathers)
			{
				if (f->creates_circle(t)) return true;
			}
			return false;
		}

		void add_father(TreeNode<T>* new_father)
		{
			fathers.push_back(new_father);
		}

	private:

		bool contains(const T& t)
		{
			return data == t;
		}

		std::vector<TreeNode*> fathers;
		T data;
	};
public:
	bool add(const T& t1)
	{
		auto [it, succ] = data_map.insert({ t1,std::make_unique<TreeNode<T>>(t1) });
		return succ;
	}

	bool add(const T& t1, const T& insert_where)
	{
		auto it = data_map.find(insert_where);
		assert(it != data_map.end());
		auto& father = it->second;

		it = data_map.find(t1);
		if (it == data_map.end())
		{
			auto [it2, succ] = data_map.insert({ t1,std::make_unique<TreeNode<T>>(t1) });
			assert(succ);
			it = it2;
		}

		if (father->creates_circle(t1)) return false;
		it->second->add_father(father.get());
		return true;
	}

	std::unordered_map<T, std::unique_ptr<TreeNode<T>>> data_map;

};