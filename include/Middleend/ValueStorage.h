#pragma once

#include "Ast.h"
#include <tuple>
#include <vector>

#define RETURN(r) ret(r);return;
#define RETURN_VAL(r,val) ret(r); return val;
#define RETURN_WITH_TRUE(r) ret(r); return true;

// TODO: pointer to derived, should be saved
template<typename T>
class ValueStorage
{
public:
	ValueStorage(IAstVisitor* visitor)
		:m_visitor(visitor) {}
	void ret(T&& arg) { m_elem = std::move(arg); }
	void ret(T& arg) { m_elem = arg; }
	
	template<typename TNode>
	T& get(std::unique_ptr<TNode>& node) { node->accept(*m_visitor); return m_elem; }
	template<typename TNode>
	T& get(TNode& node) { node.accept(*m_visitor); return m_elem; }
	T& retrieve() { return m_elem; }
private:
	T m_elem = { };
	IAstVisitor* m_visitor;
};

template<typename T, typename... TStackData>
class ValueStorageStack : public ValueStorage<T>
{
public:
	ValueStorageStack(IAstVisitor* visitor)
		:ValueStorage(visitor) {}
	void pass_params(const TStackData&... d)
	{
		stack_data = std::make_tuple(d...);
	}

	template<typename TNode>
	T& get_with_params(TNode& node, const TStackData&... d)
	{
		pass_params(d...);
		return get(node);
	}

	std::tuple<TStackData...> get_params()
	{
		return stack_data;
	}

private:
	std::tuple<TStackData...> stack_data;
};
