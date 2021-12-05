#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"

// Checks if all default types in a generic list are at the end
// Also checks something like this: struct A<T,T> <-- two times the same generic type: not allowed
// Also checks something like this: struct A<T,T2=T> <-- not allowed
class DefaultTypeArgChecker : public IAstVisitor,ValueStorage<Token*>
{
public:
	DefaultTypeArgChecker(Scopes& sc)
		:scopes(sc),ValueStorage<Token*>(this) {}

private:
	Scopes& scopes;
	std::string* must_not_contain=nullptr;

	virtual void visit(CollectionStmt& coll_def) override;
	virtual void visit(BaseTypeSpec& bts) override;
	virtual void visit(NamespaceStmt& ns) override;
};

void check_default_type_arg(NamespaceStmt& ns, Scopes& sc);