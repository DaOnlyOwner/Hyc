#include "SymbolTable.h"
#include <algorithm>

bool SymbolTable::Add(Variable&& s)
{
	return m_variables.Add(s.Name, std::move(s));
}

bool SymbolTable::Add(Function&& s)
{
	auto vec = m_functions.Get(s.Name);
	// Function name not present.
	if (vec == nullptr)
	{
		std::vector<Function> newVec = { s };
		m_functions.Add(s.Name, std::move(newVec));
	}
	// Function name exists, but it might be an overload.
	else
	{
		auto itArgs = std::find(vec->begin(), vec->end(), s.ArgumentTypeList);
		// Overload.
		if (itArgs == vec->end())
		{
			vec->push_back(std::move(s));
			return true;
		}
		// Not an overload, function name + arguments already exist.
		else return false;
	}
}

bool SymbolTable::Add(Struct&& s)
{
	m_structs.Add(s.Name, std::move(s));
}

