#include "LValueChecker.h"
#include "Messages.h"

void LValueChecker::visit(DecimalLiteralExpr& lit)
{
	RETURN(false);
}

void LValueChecker::visit(IntegerLiteralExpr& lit)
{
	RETURN(false);
}

void LValueChecker::visit(BinOpExpr& bin_op)
{
	bool lhs = get(bin_op.lh);
	bool rhs = get(bin_op.rh);
	switch (bin_op.op.type)
	{
	case Token::Specifier::Hashtag:
	case Token::Specifier::Equal:
		if (!lhs) // not an lvalue, cannot assign to it
		{
			Messages::inst().trigger_8_e1(bin_op.op, bin_op.lh->as_str());
			RETURN(false);
		}
		RETURN(true);
	case Token::Specifier::MemAccess:
	case Token::Specifier::Dot:
		RETURN(true);
	default:
		RETURN(false);
	}
}

void LValueChecker::visit(PrefixOpExpr& pre_op)
{
	if (pre_op.op.type == Token::Specifier::Asterix)
	{
		RETURN(true);
	}
	RETURN(false);
}

void LValueChecker::visit(PostfixOpExpr& post_op)
{
	RETURN(false);
}

void LValueChecker::visit(IdentExpr& ident)
{
	RETURN(true);
}

void LValueChecker::visit(FuncCallExpr& func_call_expr)
{
	RETURN(false);
}

void LValueChecker::visit(ImplicitCastExpr& ice)
{
	RETURN(true);
}

void LValueChecker::visit(ArraySubscriptExpr& subs)
{
	RETURN(true);
}

void LValueChecker::visit(TernaryExpr& tern)
{
	bool snd = get(tern.snd);
	bool trd = get(tern.trd);
	RETURN(snd && trd);
}

void check_lvalues(Scopes& sc, NamespaceStmt& ns)
{
	LValueChecker lvc(sc);
	ns.accept(lvc);
}
