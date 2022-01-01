#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "Tree.h"

class CircularEmbedChecker : public IAstVisitor
{
public:
	CircularEmbedChecker(Tree<TypeDefStmt*>& type_hier, Scopes& sc)
		:type_hier(type_hier),scopes(sc){}

private:
	Tree<TypeDefStmt*>& type_hier;
	Scopes& scopes;

	virtual void visit(FuncDefStmt& fds){}
	virtual void visit(TypeDefStmt& td) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(UnionDeclStmt& udecl) override;
	TypeDefStmt* embedded_in = nullptr;

};

void check_circular_embed(NamespaceStmt& ns, Tree<TypeDefStmt*>& tree, Scopes& sc);

