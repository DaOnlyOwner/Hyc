#pragma once
#include "Ast.h"
#include "LLVMBackendDefs.h"
#include "Tree.h"

class LLVMBackendTypeCollector : public IAstVisitor
{
public:
	LLVMBackendTypeCollector(LLVMBackendInfo& be)
		:be(be){}
private:
	LLVMBackendInfo& be;

	virtual void visit(TypeDefStmt& stmt) override;
	virtual void visit(FuncDefStmt& def) override;
};

void llvm_collect_types(LLVMBackendInfo& be, NamespaceStmt& ns);