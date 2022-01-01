#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "Tree.h"
#include "ValueStorage.h"
#include <functional>
#include <optional>

using gen_func_t = std::optional<uptr<Stmt>>(const std::vector<DeclStmt*>&, Scopes&, NamespaceStmt&);
using gen_arglist_t = std::vector<uptr<DeclStmt>>(const Type& t);

class GenerateOps : public IAstVisitor,public ValueStorage<std::optional<uptr<Stmt>>>
{
public:
	GenerateOps(Scopes& sc, NamespaceStmt& ns, std::function<gen_func_t>& gen_func_struct,
		std::function<gen_func_t>& gen_func_union,
		std::function<gen_arglist_t>& gen_arg_list)
		:scopes(sc),ns(ns), gen_func_struct(gen_func_struct),gen_func_union(gen_func_union), gen_arg_list(gen_arg_list), ValueStorage(this)//,member_rel(member_rel) 
	{}

	bool has_generated_op()
	{
		return generated_op;
	}

private:
	Scopes& scopes;
	//Tree<TypeDefStmt*>& member_rel;
	NamespaceStmt& ns;
	std::function<gen_func_t>& gen_func_struct;
	std::function<gen_func_t>& gen_func_union;
	std::function<gen_arglist_t>& gen_arg_list;
	bool generated_op = false; // Only useful when called on a single type

	virtual void visit(FuncDefStmt& f) override {}
	virtual void visit(TypeDefStmt& td) override;
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(NamespaceStmt& ns) override;
};