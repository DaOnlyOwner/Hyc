#include "SymbolTable.h"

bool SymbolTable::add(Function* fn)
{
	auto maybe_fns = m_functions.get(fn->name);
	if (maybe_fns == nullptr)
    {
        std::vector<std::unique_ptr<Function>> funcs;
        funcs.push_back(std::unique_ptr<Function>(fn));
        m_functions.insert(std::string(fn->name),std::move(funcs));
    }
	// Might be an overload
	else
	{
		auto& fns = *maybe_fns;
		if (std::find_if(fns.begin(), fns.end(), [fn](auto& uptr_fn) {return fn == uptr_fn.get();}) == fns.end())
		{
			fns.push_back(std::unique_ptr<Function>(fn));
		}
		else return false;
	}
	return true;
}
