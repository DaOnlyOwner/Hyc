#pragma once
#include <string>


class Type
{
public:
	Type(const std::string& name)
		:m_name(name) {}
	Type() = default;
	const std::string& get_name() const { return m_name; }
protected:
	std::string m_name = "";
};

class ErrorType : public Type
{
public:
	ErrorType():Type("ErrorType"){}
};

extern ErrorType error_type;