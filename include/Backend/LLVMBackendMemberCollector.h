#pragma once
#include "Ast.h"
#include "LLVMBackendDefs.h"
#include "Tree.h"
#include "ValueStorage.h"
#include "Scopes.h"

class LLVMBackendMemberCollector : public IAstVisitor, ValueStorage<llvm::Type*>
{
public:
	LLVMBackendMemberCollector(LLVMBackendInfo& be, Tree<TypeDefStmt*>& type_hier, Scopes& sc)
		:be(be),type_hier(type_hier),scopes(sc),ValueStorage(this){}
private:
	virtual void visit(TypeDefStmt& stmt) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(UnionDeclStmt& udecl) override;
	virtual void visit(FuncDefStmt& def) override;
	LLVMBackendInfo& be;
	Tree<TypeDefStmt*>& type_hier;
	Scopes& scopes;
	TypeDefStmt* embedded_in = nullptr;
};

void llvm_collect_member(LLVMBackendInfo& be, Tree<TypeDefStmt*>& type_hier, Scopes& sc, NamespaceStmt& ns);