#pragma once
#include "Ast.h"
#include "LLVMBackendDefs.h"
#include "Tree.h"

class LLVMBackendTypeCollector : public IAstVisitor
{
public:
	LLVMBackendTypeCollector(LLVMBackendInfo& be, Tree<TypeDefStmt*>& type_hier)
		:be(be),type_hier(type_hier){}
private:
	LLVMBackendInfo& be;
	Tree<TypeDefStmt*>& type_hier;

	virtual void visit(TypeDefStmt& stmt) override;
	virtual void visit(FuncDefStmt& def) override;
};

void llvm_collect_types(LLVMBackendInfo& be, Tree<TypeDefStmt*>& type_hier, NamespaceStmt& ns);