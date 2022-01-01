#pragma once
#include "Ast.h"
#include "LLVMBackendDefs.h"
#include "Tree.h"
#include "ValueStorage.h"
#include "Scopes.h"

class LLVMBackendMemberCollector : public IAstVisitor, ValueStorage<llvm::Type*>
{
public:
	LLVMBackendMemberCollector(LLVMBackendInfo& be, Scopes& sc)
		:be(be),scopes(sc),ValueStorage(this){}
private:
	virtual void visit(TypeDefStmt& stmt) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(UnionDeclStmt& udecl) override;
	virtual void visit(FuncDefStmt& def) override;
	LLVMBackendInfo& be;
	Scopes& scopes;
};

void llvm_collect_member(LLVMBackendInfo& be, Scopes& sc, NamespaceStmt& ns);