#include "TypeChecker.h"
#include "TypeCreator.h"
#include "Messages.h"


namespace
{
	bool is_pred(const Scopes& scopes, const Type& t)
	{
		return t.is_base_type() && scopes.is_type_predefined(*t.get_base_type());
	}
}

void TypeChecker::visit(FloatLiteralExpr& lit)
{
	assert(false);
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	RETURN(Type(scopes.get_base_type("int")));
}

void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto tlh = get(bin_op.lh);
	auto trh = get(bin_op.rh);
	bool was_void = false;
	if (is_pred(scopes,tlh))
	{	
		if (scopes.get_predefined_type(*tlh.get_base_type()) == PredefinedType::Void)
		{
			// No operation specified for type void.
			Messages::inst().trigger_6_e5(bin_op.op, true);
			was_void = true;
		}
	}

	if (is_pred(scopes, trh))
	{
		if (scopes.get_predefined_type(*trh.get_base_type()) == PredefinedType::Void)
		{
			// No operation specified for type void.
			Messages::inst().trigger_6_e5(bin_op.op, false);
			was_void = true;
		}
	}
	if (was_void)
	{
		RETURN(trh);
	}

	if (is_pred(scopes, tlh) && is_pred(scopes, trh))
	{
		auto plh = scopes.get_predefined_type(*tlh.get_base_type());
		auto prh = scopes.get_predefined_type(*trh.get_base_type());
		auto [clh, crh] = Type::type_cast_to_more_general(plh, prh);

		switch (bin_op.op.type)
		{
		case Token::Specifier::Or:
		case Token::Specifier::Ampersand:
		case Token::Specifier::Caret:
		case Token::Specifier::ShiftLeft:
		case Token::Specifier::ShiftRight:
		case Token::Specifier::Asterix:
		case Token::Specifier::Plus:
		case Token::Specifier::Minus:
		case Token::Specifier::Slash:
		case Token::Specifier::ThreeWay:
		case Token::Specifier::Less:
		case Token::Specifier::Greater:
		case Token::Specifier::DoubleEqual:
		case Token::Specifier::NotEqual:
		{
			if (clh == ConversionType::ImplicitCasting)
			{
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), trh);
				bin_op.lh = std::move(ice);
				bin_op.sem_type = trh;
				tlh = trh;
			}
			else if (crh == ConversionType::ImplicitCasting)
			{
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.lh), tlh);
				bin_op.rh = std::move(ice);
				bin_op.sem_type = tlh;
				trh = tlh;
			}
		}
		break;
		// For this case always take the lhs
		case Token::Specifier::PercentEqual:
		case Token::Specifier::CaretEqual:
		case Token::Specifier::SlEqual:
		case Token::Specifier::SrEqual:
		case Token::Specifier::OrEqual:
		case Token::Specifier::AmpersandEqual:
		case Token::Specifier::LessEql:
		case Token::Specifier::GreaterEql:
		case Token::Specifier::Equal:
		case Token::Specifier::PlusEqual:
		case Token::Specifier::MinusEqual:
		case Token::Specifier::AsterixEqual:
		case Token::Specifier::SlashEqual:
		{
			auto lhptr = dynamic_cast<IdentExpr*>(bin_op.lh.get());
			if (!lhptr)
			{
				// Needs to be an identifier.
				Messages::inst().trigger_6_e2(bin_op.op, bin_op.lh->as_str());
				bin_op.sem_type = trh;
				RETURN(trh); // Return a random type on error.
			}
			if (plh != prh)
			{
				// Cast the rhs to the lhs.
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), tlh);
				bin_op.rh = std::move(ice);
				bin_op.sem_type = tlh;
				RETURN(tlh);
			}
		}
		break;
		default:
			assert(false);
		}

		FuncDefStmt* fds = scopes.get_func(fmt::format("${}", bin_op.op.text), { tlh,trh });
		if (fds == nullptr)
		{
			Messages::inst().trigger_6_e1(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN(trh);
		}
		RETURN(fds->decl->ret_type->semantic_type);
	}

	// Pointer arithmetic
	if ((tlh.is_pointer_type() && is_pred(scopes,trh))
		||(trh.is_pointer_type() && is_pred(scopes,tlh)))
	{
		auto& ptrtype = tlh.is_pointer_type() ? tlh : trh;
		//auto& basetype = tlh.is_base_type() ? tlh : trh;

		if (bin_op.op.type != Token::Specifier::Plus && bin_op.op.type != Token::Specifier::Minus)
		{
			// Only plus and minus op are specified for pointer arithmetic
			Messages::inst().trigger_6_e3(bin_op.op);
		}
		bin_op.sem_type = ptrtype;
		RETURN(ptrtype);
	}
	
	if (tlh.is_pointer_type() && trh.is_pointer_type())
	{

		if (bin_op.op.type == Token::Specifier::Equal
			|| bin_op.op.type == Token::Specifier::NotEqual)
		{
			RETURN(Type(scopes.get_base_type("bool")));
		}

		// Every operation except == and != are not allowed on pointers
		Messages::inst().trigger_6_e4(bin_op.op, tlh.as_str(), trh.as_str());
		bin_op.sem_type = trh;
		RETURN(trh);
	}

	std::string operator_name = fmt::format("${}", bin_op.op.text);
	std::vector<Type> args = { tlh,trh };
	FuncDeclStmt* fds = scopes.get_func(operator_name, args);
	bin_op.sem_bin_op = fds;
	RETURN(fds->ret_type->semantic_type);
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
}

void TypeChecker::visit(IdentExpr& ident)
{
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
}

void TypeChecker::visit(FuncCallExpr& func_call_expr)
{
}

void TypeChecker::visit(FuncDeclStmt& func_decl)
{
}

void TypeChecker::visit(FuncDefStmt& func_def_stmt)
{
}

void TypeChecker::visit(CollectionStmt& coll_def)
{
}

void TypeChecker::visit(ReturnStmt& ret_stmt)
{
}

void TypeChecker::visit(ExprStmt& expr_stmt)
{
}

void TypeChecker::visit(DeclStmt& decl_stmt)
{
}

void TypeChecker::visit(PointerTypeSpec& pt_spec)
{
}

void TypeChecker::visit(BaseTypeSpec& bt_spec)
{
}

void TypeChecker::visit(ArrayTypeSpec& at_spec)
{
}

void TypeChecker::visit(FptrTypeSpec& fptr)
{
}

void TypeChecker::visit(ScopeTypeSpec& scope_spec)
{
}

void TypeChecker::visit(ImplicitCastExpr& ice)
{
}

void TypeChecker::visit(IfStmt& if_stmt)
{
}

void TypeChecker::visit(WhileStmt& while_stmt)
{
}

void TypeChecker::visit(ContinueStmt& cont_stmt)
{
}

void TypeChecker::visit(ArraySubscriptExpr& subs)
{
}

void TypeChecker::visit(TernaryExpr& tern)
{
}

void TypeChecker::visit(MatchStmt& match)
{
}

void TypeChecker::visit(ScopeStmt& sc)
{
}
