#pragma once
#include "Ast.h"
#include "Scopes.h"
#include "Tree.h"
#include "ValueStorage.h"
#include <functional>
#include <optional>

using gen_func_t = std::optional<uptr<Stmt>>(TypeDefStmt* t, DeclStmt*, Scopes&, NamespaceStmt&);
using gen_arglist_t = std::vector<FuncArg>(const Type& t);

class GenerateOps : public IAstVisitor,public ValueStorage<std::optional<uptr<Stmt>>>
{
public:
	GenerateOps(Scopes& sc, NamespaceStmt& ns, std::function<gen_func_t>& gen_func_struct,
		std::function<gen_func_t>& gen_func_union,
		std::function<gen_arglist_t>& gen_arg_list,
		const std::string& op_name)
		:scopes(sc),ns(ns), gen_func_struct(gen_func_struct),gen_func_union(gen_func_union), gen_arg_list(gen_arg_list), op_name(op_name), ValueStorage(this)//,member_rel(member_rel) 
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
	std::string op_name;
	bool generated_op = false; // Only useful when called on a single type
	TypeDefStmt* current_def = nullptr;

	virtual void visit(FuncDefStmt& f) override {}
	virtual void visit(TypeDefStmt& td) override;
	bool gen_union_destr(std::unique_ptr<FuncDefStmt, std::default_delete<FuncDefStmt>>& fds, TypeDefStmt& td);
	bool gen_struct_movecopy(TypeDefStmt& td, std::unique_ptr<FuncDefStmt, std::default_delete<FuncDefStmt>>& fds);
	bool gen_struct_destr(TypeDefStmt& td, FuncDefStmt* fds);
	void handle_memcpy_op_gen(const int64_t& end, const int64_t& start, TypeDefStmt& td, std::unique_ptr<Stmt>& gen_stmt);
	virtual void visit(DeclStmt& decl) override;
	virtual void visit(NamespaceStmt& ns) override;
};

void gen_op(Scopes& sc,
	NamespaceStmt& ns,
	std::function<gen_func_t>& gen_func_struct,
	std::function<gen_func_t>& gen_func_union,
	std::function<gen_arglist_t>& gen_arg_list,
	const std::string& name);