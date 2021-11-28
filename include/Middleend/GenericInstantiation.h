#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "CodePaster.h"
#include "ValueStorage.h"

class GenericInst : public IAstVisitor, ValueStorage<bool>
{
public:
	GenericInst(Scopes& scopes, NamespaceStmt& stmt, size_t entry_stmt = 0)
		:scopes(scopes),top_level(stmt),ValueStorage<bool>(this) {}

	void instantiate(BaseTypeSpec& bt)
	{
		bt.accept(*this);
	}

	size_t get_entry_stmt() { return entry_stmt; }

private:
	virtual void visit(NamespaceStmt& stmt) override;
	virtual void visit(FuncDefStmt& stmt) override;
	virtual void visit(DeclStmt& stmt) override;
	virtual void visit(CollectionStmt& stmt) override;
	virtual void visit(BaseTypeSpec& bt_spec) override;
	virtual void visit(struct IfStmt& if_stmt) override;
	virtual void visit(struct WhileStmt& while_stmt) override;
	virtual void visit(struct ForStmt& for_stmt) override;
	virtual void visit(struct MatchStmt& match) override;

	Scopes& scopes;
	NamespaceStmt& top_level;
	std::vector<GenericInfo>* generic_params;
	bool new_elem = false;
	size_t entry_stmt = 0;
};

void instantiate_generic(BaseTypeSpec& bt, Scopes& scopes, NamespaceStmt& ns);
//std::pair<bool, GenericInst> instantiate_generic(Scopes& scopes, NamespaceStmt& ns,size_t entry_stmt);