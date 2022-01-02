#include "OperatorGenerationFuncs.h"
#include "GenerateOperators.h"
#include <functional>

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_struct(TypeDefStmt* t, const std::vector<DeclStmt*>& tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist,"del");
	assert(tv.size() == 1);
	const auto& arg_t = tv[0]->type;
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
			auto rh = std::make_unique<IdentExpr>(tv[0]->name);
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

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_union(TypeDefStmt* t, const std::vector<DeclStmt*>& tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist,"del");
	assert(tv.size() == 1);
	const auto& arg_t = tv[0]->type;
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
