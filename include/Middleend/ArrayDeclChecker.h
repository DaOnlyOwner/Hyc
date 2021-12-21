// This should be completely wrong, I don't need it...
#pragma once
#include "Ast.h"
#include "Scopes.h"

class ArrayDeclChecker : public IAstVisitor
{
public:
	ArrayDeclChecker(Scopes& scopes, NamespaceStmt& ns)
		:scopes(scopes), ns(ns) {}

	virtual void visit(TypeDefStmt& cs) override {}
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(FuncDefStmt& func_def) override;
	virtual void visit(ArrayTypeSpec& array_type_spec) override;
	virtual void visit(BaseTypeSpec& bt) override;
	virtual void visit(PointerTypeSpec& pts) override;
	virtual void visit(FptrTypeSpec& fptr) override;


private:
	Scopes& scopes;
	NamespaceStmt& ns;
	bool was_not_array = true;
};

void check_array_decl(NamespaceStmt& ns, Scopes& scopes);
