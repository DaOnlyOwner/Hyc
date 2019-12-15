#pramga once
#include <string>

class MetaType
{
public:
	virtual unsigned int get_size() = 0;
	std::string& get_name() { return m_name; }
	void set_name(const std::string& name) { m_name = name; }
protected:
	std::string m_name;
}