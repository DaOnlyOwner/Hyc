#pragma once

#include "Ast.h"

#define RETURN(r) ret(r);return;

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
private:
	T m_elem = { };
	IAstVisitor* m_visitor;
};