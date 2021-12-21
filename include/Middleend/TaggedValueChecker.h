#pragma once
#include "Ast.h"
#include "Scopes.h"
#include <cstdint>

// TODO: One set tagged value -> all need to be set.
class TaggedValueChecker : public IAstVisitor
{
public:
	TaggedValueChecker(size_t entry) 
		:entry(entry){}

private:
	uint64_t tagged_val = 0;
	size_t entry = 0;

	virtual void visit(NamespaceStmt& ns) override;
	virtual void visit(FuncDefStmt& func_def) override {}
	virtual void visit(TypeDefStmt& td) override;
	virtual void visit(UnionDeclStmt& udecl) override;
};

void check_tagged_values(NamespaceStmt& ns, size_t entry);