#include "OperatorGenerationFuncs.h"
#include "GenerateOperators.h"
#include <functional>

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_struct(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist,"del");
	const auto& arg_t = decl->type;
	auto pred = arg_t.to_pred(scopes);
	if (!pred.has_value() && arg_t.is_base_type())
	{
		auto bt = arg_t.get_base_type();
		bt->accept(go);
		if (go.has_generated_op())
		{
			auto lh = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "a"));
			lh->sem_type = t;
			//lh->sem_type.promote_pointer();
			auto rh = std::make_unique<IdentExpr>(decl->name);
			//rh->sem_type = tv[0]->type;
			auto bi = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lh),std::move(rh));
			//pe->sem_type = tv[0]->type.with_pointer();
			auto del = std::make_unique<DelOpExpr>(std::move(bi));
			//del->sem_type = scopes.get_type("void");
			return std::make_unique<ExprStmt>(std::move(del));
		}
		return std::nullopt;
	}
	return nullptr;
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_union(TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist,"del");
	const auto& arg_t = decl->type;
	auto pred = arg_t.to_pred(scopes);
	if (!pred.has_value() && arg_t.is_base_type())
	{
		auto bt = arg_t.get_base_type();
		bt->accept(go);
		if (go.has_generated_op())
		{
			auto rh = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "matched"));
			//rh->sem_type = t;
			//rh->sem_type.promote_pointer();
			auto pref = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(rh));
			auto del = std::make_unique<DelOpExpr>(std::move(pref));
			//del->sem_type = scopes.get_type("void");
			return std::make_unique<ExprStmt>(std::move(del));
		}
		return std::nullopt;
	}
	return nullptr;
}

std::vector<std::unique_ptr<DeclStmt>> op_gen_funcs::gen_destructor_arglist(const Type& t)
{
	auto decl = std::make_unique<DeclStmt>(t.to_ast(), Token(Token::Specifier::Ident, "a"));
	decl->type = t;
	std::vector<uptr<DeclStmt>> out;
	out.reserve(1);
	out.push_back(std::move(decl));
	return out;
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_copy_struct(TypeDefStmt* t, DeclStmt* tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_copy_struct;
	return gen_generic_struct(gen_struct, "=", t, tv, scopes, ns);
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_move_struct(TypeDefStmt* t, DeclStmt* tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_move_struct;
	return gen_generic_struct(gen_struct, "#", t, tv, scopes, ns);
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_generic_struct(std::function<gen_func_t>& gen_struct, const std::string& name,
	TypeDefStmt* t, DeclStmt* decl, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_union = gen_copy_move_union;
	std::function<gen_arglist_t> gen_arglist = gen_copy_move_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist, name);
	const auto& arg_t = decl->type;
	auto pred = arg_t.to_pred(scopes);
	if (!pred.has_value())
	{
		auto bt = arg_t.get_base_type();
		bt->accept(go);
	}

	if (pred.has_value() || go.has_generated_op())
	{
		auto lh_a = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "a"));
		auto rh_a = std::make_unique<IdentExpr>(decl->name);
		auto bi_a = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lh_a), std::move(rh_a));
		auto lh_b = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "b"));
		auto rh_b = std::make_unique<IdentExpr>(decl->name);
		auto bi_b = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lh_b), std::move(rh_b));
		auto cpy = std::make_unique<BinOpExpr>(Token(Token::Specifier::Equal,"="),std::move(bi_a),std::move(bi_b));
		return std::make_unique<ExprStmt>(std::move(cpy));
	}
	return std::nullopt;	
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_copy_move_union(TypeDefStmt* t, DeclStmt* tv, Scopes& scopes, NamespaceStmt& ns)
{
	// NOT USED FOR COPY AND MOVE OPERATOR
	return {};
}

std::vector<std::unique_ptr<DeclStmt>> op_gen_funcs::gen_copy_move_arglist(const Type& t)
{
	auto decl_a = std::make_unique<DeclStmt>(t.to_ast(), Token(Token::Specifier::Ident, "a"));
	decl_a->type = t;
	auto decl_b = std::make_unique<DeclStmt>(t.to_ast(), Token(Token::Specifier::Ident, "b"));
	decl_b->type = t;
	std::vector<uptr<DeclStmt>> out;
	out.reserve(2);
	out.push_back(std::move(decl_a));
	out.push_back(std::move(decl_b));
	return out;
}




