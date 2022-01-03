#pragma once
#include "Ast.h"
#include <memory>
#include "Scopes.h"
#include <optional>

namespace op_gen_funcs
{
	using gen_func_t = std::optional<uptr<Stmt>>(TypeDefStmt* t, DeclStmt*, Scopes&, NamespaceStmt&);
	std::optional<std::unique_ptr<Stmt>> gen_destructor_struct(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns);
	std::optional<std::unique_ptr<Stmt>> gen_destructor_union(TypeDefStmt* t, DeclStmt* decl,Scopes& scopes, NamespaceStmt& ns);
	std::vector<std::unique_ptr<DeclStmt>> gen_destructor_arglist(const Type& t);

	std::optional<std::unique_ptr<Stmt>> gen_copy_struct(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns);
	std::optional<std::unique_ptr<Stmt>> gen_move_struct(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns);
	std::optional<std::unique_ptr<Stmt>> gen_generic_struct(std::function<gen_func_t>& gen_struct, const std::string& name,
		TypeDefStmt* t, DeclStmt* tv, Scopes& scopes, NamespaceStmt& ns);
	std::optional<std::unique_ptr<Stmt>> gen_copy_move_union(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns);
	std::vector<std::unique_ptr<DeclStmt>> gen_copy_move_arglist(const Type& t);

}
