#include "TypeChecker.h"
#include <cassert>
#include "DebugPrint.h"

// TODO: Completness: Finish this and also add _mul_ functions etc. in SymbolTable.

UID determine_return_type(std::vector<Function>& fns, std::vector<UID>& args_to_compare)
{
	auto it = std::find_if(fns.begin(), fns.end(), [&](Function& fnComp) {
		return fnComp.arguments == args_to_compare;
		});
	if (it != fns.end())
	{
		Debug("nope, no compatible function found. userbug");
	}

	else
	{
		return it->return_type;
	}
}


// TODO: Performance: define a function Scopes::get_primitive_type(Primitive::Specifier) which doesn't rely on lookup per string if the primitive type is clear.
void TypeChecker::visit(FloatLiteralExpr& lit)
{
	auto pair = m_scopes.get_meta_type("float");
	assert(pair.first != nullptr);
	ret(pair.second);
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	std::string suffix = Token::IntegerTypeToSuffixStr(lit.integer_literal.type);
	auto pair = m_scopes.get_meta_type(suffix);
	assert(pair.first != nullptr);
	ret(pair.second);
}

void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto typeLh = get(bin_op.lh);
	auto typeRh = get(bin_op.rh);

	std::pair<std::vector<Function>*, UID> fns;
	switch (bin_op.op.type)
	{
	case Token::Asterix:
		fns = m_scopes.get_funcs("_mul_");
	case Token::Plus:
		fns = m_scopes.get_funcs("_add_");
	case Token::Minus:
		fns = m_scopes.get_funcs("_sub_");
	case Token::Slash:
		fns = m_scopes.get_funcs("_div_");
	default:
		Debug("nope, function name not found, but should. it's a compilerbug.");
		abort();
	}
	ret(determine_return_type(*fns.first, { typeLh,typeRh }));
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	auto type = get(pre_op);
	std::pair<std::vector<Function>*, UID> fns;
	switch (pre_op.op.type)
	{
	case Token::Minus:
		fns = m_scopes.get_funcs("_pneg_"); // e.g. -2
	case Token::Plus:
		fns = m_scopes.get_funcs("_pplus_"); // e.g. +2
	default:
		Debug("Compilerbug, function should exist");
		assert(false);
	}
	ret(determine_return_type(*fns.first, { type }));
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
	assert(false);
}

void TypeChecker::visit(InferredDeclStmt& decl_inferred)
{
	auto expr_type_uid = get(decl_inferred.expr);
	MetaType& expr_type = m_scopes.get_meta_type(expr_type_uid);

	m_type_to_pattern_match = std::make_pair(expr_type, expr_type_uid);
	decl_inferred.bind_to->accept(*this);
}

void TypeChecker::visit(IdentPattern& ident)
{
	bool success = m_scopes.add(Variable(ident.ident.text,m_type_to_pattern_match.second));	
	if (!success)
	{
		Debug("Variable already exists."); abort();
	}
}

void TypeChecker::visit(IdentExpr& ident)
{
	auto ident_uid = m_scopes.get_var(ident.ident.text);
	if (ident_uid.first == nullptr) { Debug("Userbug, ident not declared"); abort(); }
	ret(ident_uid.second);
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	for (int i = 0; i < namespace_stmt.stmts.size(); i++)
	{
		auto& stmt = namespace_stmt.stmts[i];
		stmt->accept(*this);
	}
}
