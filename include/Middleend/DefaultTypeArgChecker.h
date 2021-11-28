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
	std::string* must_not_contain;

	virtual void visit(struct CollectionStmt& coll_def) override;
	virtual void visit(struct BaseTypeSpec& bts) override;
	/*
	virtual void visit(struct NamespaceStmt& namespace_stmt) override;
	virtual void visit(struct FuncDefStmt& func_def) override;
	virtual void visit(struct IfStmt& if_stmt) override;
	virtual void visit(struct WhileStmt& while_stmt) override;
	virtual void visit(struct ForStmt& for_stmt) override;
	virtual void visit(struct MatchStmt& match) override;
	virtual void visit(struct DeclStmt& decl_stmt) override;
	void visit(BaseTypeSpec& bt) override;
	*/
};

void check_default_type_arg(NamespaceStmt& ns, Scopes& sc);