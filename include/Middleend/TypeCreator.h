#pragma once
#include "Ast.h"
#include "AtomicType.h"
#include "Scopes.h"
#include "ValueStorage.h"

class TypeCreator : IAstVisitor, public ValueStorage<Type>
{
public:
	TypeCreator(TypeSpec& spec, Scopes& scopes, NamespaceStmt& ns, bool inst_generics=true)
		:spec(spec),scopes(scopes),ns(ns),ValueStorage<Type>(this), inst_generics(inst_generics) {}
	bool get_succ() { return succ; }
private:
	TypeSpec& spec;
	Scopes& scopes;
	NamespaceStmt& ns;
	bool succ = true;
	bool inst_generics = true;
	virtual void visit(PointerTypeSpec& pt_spec) override;
	virtual void visit(BaseTypeSpec& bt_spec) override;
	virtual void visit(ArrayTypeSpec& at_spec) override;
	virtual void visit(FptrTypeSpec& fptr) override;
};
std::pair<Type,bool> create_type(TypeSpec& ts, Scopes& scopes, NamespaceStmt& ns, bool instantiate_generic=true);
Type create_fptr_type(const std::vector<Type>& args, const Type& ret_t);


