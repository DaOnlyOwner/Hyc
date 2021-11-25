#pragma once
#include "Ast.h"
#include "AtomicType.h"
#include "Scopes.h"
#include "ValueStorage.h"

class TypeCreator : IAstVisitor, ValueStorage<Type>
{
public:
	TypeCreator(TypeSpec& spec, Scopes& scopes)
		:spec(spec),scopes(scopes),ValueStorage<Type>(*this) {}
	std::pair<Type,bool> create_type();
private:
	TypeSpec& spec;
	Scopes& scopes;
	ScopeTypeSpec* father = nullptr;
	bool succ;
	virtual void visit(struct PointerTypeSpec& pt_spec) override;
	virtual void visit(struct BaseTypeSpec& bt_spec) override;
	virtual void visit(struct ArrayTypeSpec& at_spec) override;
	virtual void visit(struct FptrTypeSpec& fptr) override;
};

std::pair<Type,bool> create_type(TypeSpec& ts, Scopes& scopes);


