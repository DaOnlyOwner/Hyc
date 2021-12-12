#pragma once
#include "Ast.h"
#include "ValuePtr.h"
#include "Scopes.h"
#include "ValueStorage.h"

class CodePaster : IAstVisitor, ValueStorage<uptr<TypeSpec>>
{
public:
	CodePaster(Stmt& stmt, Scopes& scopes, const std::vector<uptr<TypeSpec>>& to_paste, NamespaceStmt& top_level)
		:stmt(stmt), scopes(scopes), to_paste(to_paste),top_level(top_level),ValueStorage<uptr<TypeSpec>>(this) {}

	void paste()
	{
		stmt.accept(*this);
	}

private:
	virtual void visit(FuncDefStmt& stmt) override;
	virtual void visit(DeclStmt& stmt) override;
	virtual void visit(CollectionStmt& stmt) override;
	void visit(BaseTypeSpec& bt) override;

	Scopes& scopes;
	Stmt& stmt;
	NamespaceStmt& top_level;
	const std::vector<uptr<TypeSpec>>& to_paste;
	std::vector<GenericInfo>* must_replace;
};

std::string get_str(const std::string& name, const std::vector<uptr<TypeSpec>>& to_paste);