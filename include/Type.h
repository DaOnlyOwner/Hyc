#pragma once
#include <string>

class Type
{
public:
	Type(const std::string& name)
		:m_name(name) {}
	std::string& get_name() { return m_name; }
	void set_name(const std::string& name) { m_name = name; }
protected:
	std::string m_name;
};