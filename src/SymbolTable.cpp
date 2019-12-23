#include "SymbolTable.h"

bool SymbolTable::add(Function* fn)
{
	auto maybe_fns = m_functions.get(fn->name);
	if (maybe_fns == nullptr) m_functions.insert(std::string(fn->name), { std::unique_ptr<Function>(fn) });
	// Might be an overload
	else
	{
		auto& fns = *maybe_fns;
		if (std::find_if(fns.begin(), fns.end(), [fn](auto& uptrFn) {return fn == uptrFn.get();}) == fns.end())
		{
			fns.push_back(std::unique_ptr<Function>(fn));
		}
		else return false;
	}
	return true;
}
