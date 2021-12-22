#pragma once

#include "SymbolTable.h"
#include <vector>
#include "Primitive.h"
#include <unordered_map>
#include <cstdint>

/*
	r
  / .. \
 c1    cn	=> [r,c1, descendants of c1, c2, descendants of c2, ..., cn, descendants of cn]
/_\ .. /_\

*/

class Scopes
{
public:
	Scopes();
	Scopes(const Scopes&) = delete;
	Scopes& operator=(const Scopes&) = delete;

	Scopes(Scopes&&) = default;
	Scopes& operator=(Scopes&&) noexcept = default;


	//Variable* add(Variable* v) { return get_current_entry().table.add(v); }
	//Type* add(Type* mt) { return get_current_entry().table.add(mt); }
	bool add(FuncDefStmt* fn) { return top_level.add(fn); }
	bool add(TypeDefStmt* td) {	return top_level.add(td); }
	bool add(TypeDefStmt* td, DeclStmt* decl, int idx) { return top_level.add(td, decl,idx); }
	bool add(TypeDefStmt* td, UnionDeclStmt* decl) { return top_level.add(td, decl); }
	bool add(DeclStmt* decl);
	bool add(const std::string& name, llvm::Value* val) { return get_current_entry().table.add(name, val); }
	//bool add_to_existing(const std::string& name, class AllocaInst* inst);

	//Variable* get_var(const std::string& name);
	//Type* get_type(const std::string& name);
	bool is_type_defined(const std::string& t) const { return top_level.get_type(t) != nullptr; }
	bool is_type_predefined(TypeDefStmt* bt) const { return std::find_if(predefined_types.begin(), predefined_types.end(), [&](auto& p) {return &p == bt; }) != predefined_types.end(); }
	PredefinedType get_predefined_type(TypeDefStmt* bt) const { auto it = coll_to_predef.find(bt); assert(it != coll_to_predef.end()); return it->second; }


	TypeDefStmt* get_type(const std::string& name) { return top_level.get_type(name); }
	DeclStmt* get_decl_for(TypeDefStmt* cs, const std::string& name) { return top_level.get_decl_for(cs, name); }
	size_t get_decl_idx_for(TypeDefStmt* cs, const std::string& name) { return top_level.get_decl_idx_for(cs, name); }
	UnionDeclStmt* get_union_decl_for(TypeDefStmt* td, const std::string& name) { return top_level.get_union_decl_for(td, name); }
	DeclStmt* get_variable(const std::string& name);
	llvm::Value* get_value(const std::string& name);
	//class AllocaInst* get_alloca_inst(const std::string& name);
	
	template<typename Pred>
	FuncDefStmt* get_func(const std::string& name, Pred pred)
	{
		return top_level.get_func(name, pred);
	}

	/*std::vector<Function> get_all_funcs(const std::string& name); */


	void ascend();
	// Expands the tree, allocates a new node and descends to it
	int64_t expand();

	// Descends one child after the other and - completely iterated -, wraps around and starts at the root again.
	// This is really fast - just an increment operation because of the way the entries are located in the vector.
	void descend();
	void descend(int64_t nthChild);

	void go_to_root() { m_current_index = 0; }
	bool go_to_father(int amount);

private:
	struct t_entry
	{
		t_entry(int64_t father, SymbolTable&& table)
			:father(father),
			table(std::move(table)) {}
		int64_t father;
		SymbolTable table;
		std::vector<int64_t> children{};
		int current_child = 0;
	};

	SymbolTable top_level;
	std::vector<t_entry> m_collection;
	int64_t m_current_index = -1;
	t_entry& get_current_entry() 
	{
		return m_collection[m_current_index];
	}

	const t_entry& get_current_entry() const
	{
		return m_collection[m_current_index];
	}

	const t_entry& get_top_level_entry() const
	{
		return m_collection[0];
	}

	t_entry& get_entry(int64_t index)
	{
		return m_collection[index];
	}

	static std::vector<TypeDefStmt> predefined_types;
	static std::vector<FuncDefStmt> predefined_funcs;
	static std::unordered_map<TypeDefStmt*, PredefinedType> coll_to_predef;
	static DeclStmt declTrue;
	static DeclStmt declFalse;

	static bool predefined_types_init;
};


