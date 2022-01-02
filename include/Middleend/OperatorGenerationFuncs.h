#pragma once
#include "Ast.h"
#include <memory>
#include "Scopes.h"
#include <optional>

namespace op_gen_funcs
{
	std::optional<std::unique_ptr<Stmt>> gen_destructor_struct(TypeDefStmt* t, const std::vector<DeclStmt*>& tv, Scopes& scopes, NamespaceStmt& ns);
	std::optional<std::unique_ptr<Stmt>> gen_destructor_union(TypeDefStmt* t, const std::vector<DeclStmt*>& tv,Scopes& scopes, NamespaceStmt& ns);
	std::vector<std::unique_ptr<DeclStmt>> gen_destructor_arglist(const Type& t);
}
