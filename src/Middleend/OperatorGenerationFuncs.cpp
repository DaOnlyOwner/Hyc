#include "OperatorGenerationFuncs.h"
#include "GenerateOperators.h"
#include <functional>

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_struct(const std::vector<DeclStmt*>& tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist);
	assert(tv.size() == 1);
	const auto& arg_t = tv[0]->type;
	auto pred = arg_t.to_pred(scopes);
	if (!pred.has_value() && arg_t.is_base_type())
	{
		auto bt = arg_t.get_base_type();
		bt->accept(go);
		if (go.has_generated_op())
		{
			auto rh = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "a"));
			auto lh = std::make_unique<IdentExpr>(tv[0]->name);
			auto bi = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lh),std::move(rh));
			auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "&"), std::move(bi));
			auto del = std::make_unique<DelOpExpr>(std::move(pe));
			return std::make_unique<ExprStmt>(std::move(del));
		}
		return std::nullopt;
	}
	return nullptr;
}

std::optional<std::unique_ptr<Stmt>> op_gen_funcs::gen_destructor_union(const std::vector<DeclStmt*>& tv, Scopes& scopes, NamespaceStmt& ns)
{
	std::function<gen_func_t> gen_struct = gen_destructor_struct;
	std::function<gen_func_t> gen_union = gen_destructor_union;
	std::function<gen_arglist_t> gen_arglist = gen_destructor_arglist;
	GenerateOps go(scopes, ns, gen_struct, gen_union, gen_arglist);
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
			auto del = std::make_unique<DelOpExpr>(std::move(rh));
			return std::make_unique<ExprStmt>(std::move(del));
		}
		return std::nullopt;
	}
	return nullptr;
}

std::vector<std::unique_ptr<DeclStmt>> op_gen_funcs::gen_destructor_arglist(const Type& t)
{
	auto decl = std::make_unique<DeclStmt>(Token(Token::Specifier::Ident, "a"));
	decl->type = t;
	return { std::move(decl) };
}
