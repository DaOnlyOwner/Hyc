#include "StackCollection.h"
#include <cassert>

StackCollection::StackCollection()
{
	m_collection.emplace_back(t_entry{ 0,SymbolTable()});
}

#define AddTypeMacro(Type)\
bool StackCollection::Add( Type && s)\
{\
	return get_current_entry().Table.Add(std::move(s));\
}

AddTypeMacro(Function)
AddTypeMacro(Variable)
AddTypeMacro(Struct)


