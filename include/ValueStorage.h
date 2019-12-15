#pragma once

#include "Ast.h"

// TODO: pointer to derived, should be saved
template<typename T>
class ValueStorage
{
public:
	void ret(T&& arg) { m_elem = std::forward<T>(arg); }
	void ret(T& arg) { m_elem = std::forward<T>(arg); }
	template<typename TNode>
	T& get(TNode& node) { node->accept(*this); return m_elem; }
private:
	T m_elem;
};