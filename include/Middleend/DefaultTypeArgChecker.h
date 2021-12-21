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

	bool had_errors() { return error; }

private:
	Scopes& scopes;
	const std::string* must_not_contain=nullptr;

	virtual void visit(TypeDefStmt& coll_def) override;
	virtual void visit(BaseTypeSpec& bts) override;
	virtual void visit(NamespaceStmt& ns) override;
	virtual void visit(FuncDefStmt& def) override;

	void check(std::vector<GenericInfo>& gis, const Token& name, const std::string& t);
	bool error=false;
};

bool check_default_type_arg(NamespaceStmt& ns, Scopes& sc);