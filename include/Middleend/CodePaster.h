#pragma once
#include "Ast.h"
#include "ValuePtr.h"
#include "Scopes.h"

class CodePaster : IAstVisitor
{
public:
	CodePaster(Stmt& stmt, Scopes& scopes, const std::vector<std::string>& to_paste, NamespaceStmt& top_level)
		:stmt(stmt), scopes(scopes), to_paste(to_paste),top_level(top_level) {}

	void paste()
	{
		stmt.accept(*this);
	}

private:
	virtual void visit(FuncDeclStmt& stmt) override;
	virtual void visit(DeclStmt& stmt) override;
	virtual void visit(StructDefStmt& stmt) override;
	virtual void visit(UnionDefStmt& stmt) override;
	void visit(BaseTypeSpec& bt);

	Scopes& scopes;
	Stmt& stmt;
	NamespaceStmt& top_level;
	const std::vector<std::string>& to_paste;
	std::vector<GenericInfo>* must_replace;
};