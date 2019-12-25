#include "TypeChecker.h"
#include <cassert>
#include "DebugPrint.h"

/*MetaType* determine_return_type(std::vector<Function>& fns, std::vector<MetaType*>& args_to_compare)
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
}*/


// TODO: Performance: define a function Scopes::get_primitive_type(Primitive::Specifier) which doesn't rely on lookup per string if the primitive type is clear.
void TypeChecker::visit(FloatLiteralExpr& lit)
{
	Primitive* primitive = m_scopes.get_primitive_type(Primitive::Specifier::Double);
	assert(primitive != nullptr);
	ret(static_cast<MetaType*>(primitive));
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	Primitive* primitive = m_scopes.get_primitive_type(Primitive::from_token_specifier(lit.integer_literal.type));
	assert(primitive!=nullptr);
	ret(static_cast<MetaType*>(primitive));
}

void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto* typeLh = get(bin_op.lh);
	auto* typeRh = get(bin_op.rh);

	BinaryOperator* bin_operator = m_scopes.get_binary_operator(BinaryOperator::from_token_specifier(bin_op.op.type), [typeLh,typeRh](const BinaryOperator* bin) {
		return bin->lh == typeLh && bin->rh == typeRh;
	});

	if (bin_operator == nullptr)
	{
		Debug("Binary operator not found, userbug or compilerbug");
		abort();
	}

	ret(bin_operator->return_type);
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	auto* type = get(pre_op.lh);
	UnaryOperator* unary_operator = m_scopes.get_unary_operator(UnaryOperator::from_token_specifier(pre_op.op.type), [type](const UnaryOperator* un) {
		return un->rh == type;
		});

	if (unary_operator == nullptr)
	{
		Debug("Unary operator not found, userbug or compilerbug");
		abort();
	}

	ret(unary_operator->return_type);
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
	assert(false);
}

void TypeChecker::visit(InferredDeclStmt& decl_inferred)
{
	auto* expr_type = get(decl_inferred.expr);
	
	m_type_to_pattern_match = expr_type;
	decl_inferred.bind_to->accept(*this);
}

void TypeChecker::visit(IdentPattern& ident)
{
	bool success = m_scopes.add(new Variable(ident.ident.text,m_type_to_pattern_match));	
	if (!success)
	{
		Debug("Variable already exists."); abort();
	}
}

void TypeChecker::visit(IdentExpr& ident)
{
	auto* var = m_scopes.get_var(ident.ident.text);
	if (var == nullptr) { Debug("Userbug, ident not declared"); abort(); }
	ret(static_cast<MetaType*>(var->type));
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	m_scopes.expand();
	for (int i = 0; i < namespace_stmt.stmts.size(); i++)
	{
		auto& stmt = namespace_stmt.stmts[i];
		stmt->accept(*this);
	}
	m_scopes.ascend();
}
