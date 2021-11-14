#include "SymbolTable.h"

Function error_function{};
Variable error_variable{};

Function* SymbolTable::add(Function* fn)
{
	auto it = m_functions.find(fn->name);
	auto managed = std::unique_ptr<Function>(fn);
	if (it == m_functions.end())
	{
		std::vector<std::unique_ptr<Function>> vec;
		vec.push_back(std::move(managed));
		m_functions[fn->name] = std::move(vec);
		return fn;
	}

	auto& vec = it->second;
	if (std::find_if(vec.begin(), vec.end(), [&](auto& fn_) {return *fn == *fn_; }) == vec.end())
	{
		vec.push_back(std::move(managed));
		return fn;
	}
	return nullptr;
}
