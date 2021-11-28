#pragma once
#include "Ast.h"
#include "AtomicType.h"
#include "Scopes.h"
#include "ValueStorage.h"

class TypeCreator : IAstVisitor, public ValueStorage<Type>
{
public:
	TypeCreator(TypeSpec& spec, Scopes& scopes, NamespaceStmt& ns)
		:spec(spec),scopes(scopes),ns(ns),ValueStorage<Type>(this) {}
	bool get_succ() { return succ; }
private:
	TypeSpec& spec;
	Scopes& scopes;
	NamespaceStmt& ns;
	bool succ = true;
	virtual void visit(struct PointerTypeSpec& pt_spec) override;
	virtual void visit(struct BaseTypeSpec& bt_spec) override;
	virtual void visit(struct ArrayTypeSpec& at_spec) override;
	virtual void visit(struct FptrTypeSpec& fptr) override;
};
std::pair<Type,bool> create_type(TypeSpec& ts, Scopes& scopes, NamespaceStmt& ns);



