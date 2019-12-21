#pragma once
#include <string>

#define IMPL_VISITOR_TYPE virtual void accept(ITypeVisitor& visitor) override {visitor.visit(*this);}

class ITypeVisitor
{
public:
	virtual void visit(class OrType& or_type) = 0;
	virtual void visit(class AndType& and_type) = 0;
	virtual void visit(class Primitive& primitive) = 0;
};

class MetaType
{
public:
	MetaType(const std::string& name)
		:m_name(name) {}
	std::string& get_name() { return m_name; }
	void set_name(const std::string& name) { m_name = name; }

	virtual void accept(ITypeVisitor& visitor) = 0;

protected:
	std::string m_name;
};