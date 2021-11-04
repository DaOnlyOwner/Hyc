#include "SymbolTable.h"

extern Function error_function{};
extern Variable error_variable{};

Function* SymbolTable::add(Function&& fn)
{
	auto maybe_fns = m_functions.get(fn.name);
	if (maybe_fns == nullptr)
	{
		auto name = fn.name;
		return &(*m_functions.insert(std::move(name), { std::move(fn) }))[0];
	}
	// Might be an overload

	auto& fns = *maybe_fns;
	if (std::find_if(fns.begin(), fns.end(), [&](const auto& fn_) {return fn == fn_; }) == fns.end())
	{
		fns.push_back(std::move(fn));
		return &fns[fns.size() - 1];
	}
	return nullptr;

}
