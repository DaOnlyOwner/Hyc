#include "TypeChecker.h"
#include "TypeCreator.h"
#include "Messages.h"
#include "Token.h"

namespace
{
	bool is_pred(const Scopes& scopes, const Type& t)
	{
		return t.is_base_type() && scopes.is_type_predefined(t.get_base_type());
	}

	Type translate(Scopes& sc, PredefinedType pt)
	{
		switch (pt)
		{
		case PredefinedType::Void:
			return Type(sc.get_type("void"));
		case PredefinedType::Quad:
			return Type(sc.get_type("quad"));
		case PredefinedType::Double:
			return Type(sc.get_type("double"));
		case PredefinedType::Float:
			return Type(sc.get_type("float"));
		case PredefinedType::Int:
			return Type(sc.get_type("int"));
		case PredefinedType::Half:
			return Type(sc.get_type("half"));
		case PredefinedType::Short:
			return Type(sc.get_type("short"));
		case PredefinedType::Char:
			return Type(sc.get_type("char"));
		case PredefinedType::UInt:
			return Type(sc.get_type("uint"));
		case PredefinedType::UChar:
			return Type(sc.get_type("uchar"));
		case PredefinedType::UHalf:
			return Type(sc.get_type("uhalf"));
		case PredefinedType::UShort:
			return Type(sc.get_type("ushort"));
		case PredefinedType::Bool:
			return Type(sc.get_type("bool"));
		default:
			assert(false);
		}
	}
}

void TypeChecker::visit(FloatLiteralExpr& lit)
{
	assert(false);
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	RETURN(Type(scopes.get_type("int")));
}

// TODO: Handle =: *a = 3; <-- here *a needs to be an lvalue.
// Don't allow things like i+1 = 2;
// Everything on the lhs needs to be an lvalue.
void TypeChecker::visit(BinOpExpr& bin_op)
{
	auto tlh = get(bin_op.lh);
	auto trh = get(bin_op.rh);
	
	bool handled = handle_bin_op_void(tlh, trh, bin_op);
	handled = handled || handle_bin_op_predefined(tlh, trh, bin_op);
	handled = handled || handle_bin_op_pointer_arithmetic(tlh, trh, bin_op);
	handled = handled || handle_bin_op_pointer_types(tlh, trh, bin_op);
	handled = handled || handle_bin_op_inferred(tlh, trh, bin_op);
	handled = handled || handle_bin_op_member_acc(tlh, trh, bin_op);
	if (handled)
		handle_bin_op_overloads(tlh, trh, bin_op);
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	Type t = get(pre_op.lh);
	if (t.must_be_inferred())
		// Prefix operation: must have a type.
		Messages::inst().trigger_6_e1(pre_op.op,pre_op.lh->as_str());
	if (is_pred(scopes,t))
	{
		switch (pre_op.op.type)
		{
		case Token::Specifier::Plus:
			RETURN(t);
		case Token::Specifier::Minus:
			auto pt = scopes.get_predefined_type(t.get_base_type());
			if (Type::is_unsigned_integer(pt))
			{
				auto converted = (PredefinedType)((int)pt + 4);
				RETURN(translate(scopes, pt));
			}
			RETURN(t);
		case Token::Specifier::ExclMark:
			RETURN(Type(scopes.get_type("bool")));
		case Token::Specifier::Tilde:
			RETURN(t);
		case Token::Specifier::Asterix:
			if (!t.is_pointer_type())
			{
				// Error: no indirection for non pointer.
				Messages::inst().trigger_6_e2(pre_op.op,pre_op.lh->as_str());
				RETURN(error_type);
			}
			t.pop();
			RETURN(t);
		case Token::Specifier::Ampersand:
			t.promote_pointer();
			RETURN(t);
		}
	}

	// Overloaded
	std::string operator_name = fmt::format("${}", pre_op.op.text);
	FuncDefStmt* fds = scopes.get_func(operator_name, [&](const FuncDeclStmt& decl) {return decl.arg_list.size() == 1 && decl.arg_list[0]->type == t; });
	if (fds == nullptr)
	{
		// Error: Operator not defined for types. 
		Messages::inst().trigger_6_e3(pre_op.op,t.as_str());
		RETURN(error_type);
	}
	pre_op.sem_unary_op = fds->decl.get();
	pre_op.sem_type = fds->decl->ret_type->semantic_type;
	RETURN(fds->decl->ret_type->semantic_type);
}

void TypeChecker::visit(PostfixOpExpr& post_op)
{
	auto t = get(post_op.rh);
	if (t.must_be_inferred())
	{
		Messages::inst().trigger_6_e1(post_op.op,post_op.rh->as_str());
		RETURN(error_type);
	}
	if (post_op.op.type == Token::Specifier::DoublePlus)
	{
		post_op.sem_type = t;
		RETURN(t);
	}
	assert(false);
}

void TypeChecker::visit(IdentExpr& ident)
{
	auto* bt = scopes.get_type(ident.ident.text);
	RETURN(Type(bt));
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	scopes.descend();
	for (auto& p : namespace_stmt.stmts) p->accept(*this);
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

bool TypeChecker::handle_bin_op_pointer_arithmetic(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// Pointer arithmetic
	bool is_pointer_arith = (tlh.is_pointer_type() && is_pred(scopes, trh))
		|| (trh.is_pointer_type() && is_pred(scopes, tlh));

	if (is_pointer_arith)
	{
		auto& ptrtype = tlh.is_pointer_type() ? tlh : trh;
		auto& base_expr = tlh.is_base_type() ? bin_op.lh : bin_op.rh;

		if (bin_op.op.type != Token::Specifier::Plus
			&& bin_op.op.type != Token::Specifier::Minus
			&& bin_op.op.type != Token::Specifier::Equal
			&& bin_op.op.type != Token::Specifier::PlusEqual
			&& bin_op.op.type != Token::Specifier::MinusEqual)
		{
			// Only plus, minus and = += -= ops are specified for pointer arithmetic
			Messages::inst().trigger_6_e4(bin_op.op,tlh.as_str(),trh.as_str());
			ret(error_type);
		}

		if (bin_op.op.type == Token::Specifier::Equal)
		{
			auto* ptr = dynamic_cast<IntegerLiteralExpr*>(base_expr.get());
			if (!ptr)
			{
				// Ptrs may only be assigned integer literals
				Messages::inst().trigger_6_e5(bin_op.op);
				ret(error_type);
			}
			if (ptr && ptr->integer_literal.text != "0")
			{
				Messages::inst().trigger_6_e5(bin_op.op); // Only nullptr assignment allowed on pointers.
				ret(error_type);
			}
		}

		bin_op.sem_type = ptrtype;
		ret(ptrtype);
	}
	return is_pointer_arith;
}

bool TypeChecker::handle_bin_op_predefined(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// Handle both are predefined types.
	bool is_pred_types = is_pred(scopes, tlh) && is_pred(scopes, trh);
	if (is_pred_types)
	{
		auto plh = scopes.get_predefined_type(tlh.get_base_type());
		auto prh = scopes.get_predefined_type(trh.get_base_type());
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
		case Token::Specifier::LessEql:
		case Token::Specifier::GreaterEql:
		case Token::Specifier::DoubleEqual:
		case Token::Specifier::NotEqual:
		{
			if(Type::is_signed_integer(plh) && Type::is_unsigned_integer(prh))
			{
				// Signed op unsigned warning: cast to signed
				Messages::inst().trigger_6_w1(bin_op.op,bin_op.rh->as_str());
			}
			else if (Type::is_signed_integer(prh) && Type::is_unsigned_integer(plh))
			{
				Messages::inst().trigger_6_w1(bin_op.op, bin_op.lh->as_str());
			}


			if (clh == ConversionType::ImplicitCasting)
			{
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), trh);
				bin_op.lh = std::move(ice);
				tlh = trh;
			}
			else if (crh == ConversionType::ImplicitCasting)
			{
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.lh), tlh);
				bin_op.rh = std::move(ice);
				trh = tlh;
			}
			if (bin_op.op.type == Token::Specifier::Less
				|| bin_op.op.type == Token::Specifier::Greater
				|| bin_op.op.type == Token::Specifier::LessEql
				|| bin_op.op.type == Token::Specifier::GreaterEql
				|| bin_op.op.type == Token::Specifier::NotEqual
				|| bin_op.op.type == Token::Specifier::DoubleEqual)
			{
				Type t(scopes.get_type("bool"));
				bin_op.sem_type = t;
				RETURN(t,true);
			}

			else if (bin_op.op.type == Token::Specifier::ThreeWay)
			{
				Type t(scopes.get_type("int"));
				bin_op.sem_type = t;
				RETURN(t,true);
			}

			else if (bin_op.op.type == Token::Specifier::ShiftLeft
				|| bin_op.op.type == Token::Specifier::ShiftRight
				|| bin_op.op.type == Token::Specifier::Caret
				|| bin_op.op.type == Token::Specifier::Or
				|| bin_op.op.type == Token::Specifier::Ampersand)
			{
				auto prh = scopes.get_predefined_type(trh.get_base_type());
				if (!Type::is_integer(prh))
				{
					Messages::inst().trigger_6_e6(bin_op.op,tlh.as_str(),trh.as_str());
					bin_op.sem_type = error_type;
					RETURN(error_type,true);
				}
				bin_op.sem_type = trh;
				RETURN(trh,true);
			}

			else
			{
				bin_op.sem_type = tlh;
				RETURN(tlh,true);
			}

		}
		break;
		case Token::Specifier::DoubleAmpersand:
		case Token::Specifier::DoubleOr:
		{
			if (plh == PredefinedType::Bool && prh == PredefinedType::Bool)
			{
				bin_op.sem_type = tlh;
				RETURN(tlh);
			}
			// && and || need to have types bool as lhs and rhs
			Messages::inst().trigger_6_e7(bin_op.op,tlh.as_str(),trh.as_str());
			bin_op.sem_type = error_type;
			RETURN(error_type,true);
		}
		break;
		case Token::Specifier::PercentEqual:
		case Token::Specifier::CaretEqual:
		case Token::Specifier::SlEqual:
		case Token::Specifier::SrEqual:
		case Token::Specifier::OrEqual:
		case Token::Specifier::AmpersandEqual:
		case Token::Specifier::PlusEqual:
		case Token::Specifier::MinusEqual:
		case Token::Specifier::AsterixEqual:
		case Token::Specifier::SlashEqual:
		{
			// Explicitly write the expression:
			Token::Specifier split = Token::SplitCompound(bin_op.op.type);
			Token op_inner(split, std::to_string(bin_op.op.text[0]));
			auto inner_lhs = bin_op.lh->clone();
			auto inner_rhs = std::move(bin_op.rh);
			if (clh == ConversionType::ImplicitCasting)
			{
				inner_lhs = std::make_unique<ImplicitCastExpr>(std::move(inner_lhs), trh);
			}
			else if (crh == ConversionType::ImplicitCasting)
			{
				inner_rhs = std::make_unique<ImplicitCastExpr>(std::move(inner_rhs), tlh);
			}
			auto inner = std::make_unique<BinOpExpr>(op_inner, std::move(inner_lhs), std::move(bin_op.rh));
			auto outer = BinOpExpr(Token(Token::Specifier::Equal, "="), std::move(bin_op.lh), std::move(inner));
			bin_op = std::move(outer);
			bin_op.sem_type = trh;
			RETURN(trh, true);
		}
		// For this case always take the lhs
		case Token::Specifier::Equal:
		{
			// For e.g. a = 4 -> always take the type of the lhs and cast the rhs to the lhs.
			if (plh != prh)
			{
				// Cast the rhs to the lhs.
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), tlh);
				bin_op.rh = std::move(ice);
				bin_op.sem_type = tlh;
				RETURN(tlh, true);
			}
		}
		break;
		case Token::Specifier::KwAs:
			assert(false);
		default:
		{
			// Error: unsupported operation type for type and type. 
			Messages::inst().trigger_6_e8(bin_op.op,tlh.as_str(),trh.as_str());
			RETURN(error_type, true);
		}
		}
	}
	return is_pred_types;
}

bool TypeChecker::handle_bin_op_void(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	bool was_void = false;

	// Handle void
	if (is_pred(scopes, tlh))
	{
		if (scopes.get_predefined_type(tlh.get_base_type()) == PredefinedType::Void)
		{
			// No operation specified for type void.
			Messages::inst().trigger_6_e9(bin_op.op, bin_op.lh->as_str());
			was_void = true;
		}
	}

	if (is_pred(scopes, trh))
	{
		if (scopes.get_predefined_type(trh.get_base_type()) == PredefinedType::Void)
		{
			// No operation specified for type void.
			Messages::inst().trigger_6_e9(bin_op.op, bin_op.rh->as_str());
			was_void = true;
		}
	}
	if (was_void)
	{
		RETURN(error_type, true);
	}
	return false;
}

bool TypeChecker::handle_bin_op_pointer_types(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	if (tlh.is_pointer_type() && trh.is_pointer_type())
	{

		if (bin_op.op.type == Token::Specifier::Equal
			|| bin_op.op.type == Token::Specifier::NotEqual
			|| bin_op.op.type == Token::Specifier::DoubleEqual)
		{
			auto bool_type = Type(scopes.get_type("bool"));
			bin_op.sem_type = bool_type;
			RETURN(bool_type,true);
		}

		// Every operation except =, == and != are not allowed on pointers
		Messages::inst().trigger_6_e4(bin_op.op, tlh.as_str(), trh.as_str());
		RETURN(error_type, true);
	}
	return false;
}

bool TypeChecker::handle_bin_op_inferred(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// When the lhs has no type yet, we infer it
	if (tlh.must_be_inferred())
	{
		if (bin_op.op.type != Token::Specifier::Equal
			&& bin_op.op.type != Token::Specifier::Hashtag)
		{
			// Cannot infer type when operator is not = or #.
			Messages::inst().trigger_6_e10(bin_op.op);
			RETURN(error_type,true);
		}
	
		auto* ptrlh = dynamic_cast<IdentExpr*>(bin_op.lh.get());
		if (!ptrlh)
		{
			Messages::inst().trigger_6_e15(bin_op.op, bin_op.lh->as_str());
			RETURN(error_type, true);
		}

		// TODO: Get the decl stmt and assign a type to it.
		ptrlh->sem_type = trh;
		RETURN(trh);
	}

	// When the rhs has no type yet, it's an error
	if (trh.must_be_inferred())
	{
		// Type of rhs must be known.
		Messages::inst().trigger_6_e11(bin_op.op,bin_op.rh->as_str());
		RETURN(error_type,true);
	}
	return false;
}

bool TypeChecker::handle_bin_op_overloads(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// Overloaded operators
	std::string operator_name = fmt::format("${}", bin_op.op.text);
	FuncDefStmt* fds = scopes.get_func(operator_name, [&](const FuncDeclStmt& decl) {return decl.arg_list.size() == 2 && decl.arg_list[0]->type == tlh && decl.arg_list[1]->type == trh; });
	if (fds == nullptr)
	{
		// Error: Operator not defined for types. 
		Messages::inst().trigger_6_e4(bin_op.op,tlh.as_str(),trh.as_str());
		ret(error_type);
		return true;
	}
	bin_op.sem_bin_op = fds->decl.get();
	ret(fds->decl->ret_type->semantic_type);
	return true;
}

bool TypeChecker::handle_bin_op_member_acc(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// . -> operators
	if (bin_op.op.type == Token::Specifier::Dot
		|| bin_op.op.type == Token::Specifier::MemAccess)
	{
		auto cpy_tlh = tlh;
		if (bin_op.op.type == Token::Specifier::MemAccess)
			cpy_tlh.pop();
		if (!cpy_tlh.is_base_type())
		{
			Messages::inst().trigger_6_e13(bin_op.op,cpy_tlh.as_str()); // Pointer and other types do not have members
			RETURN(error_type,true);
		}
		auto* ident_lh = dynamic_cast<IdentExpr*>(bin_op.rh.get());
		if (ident_lh == nullptr)
		{
			// Error: a->b or a.b <-- b needs to be an identifier.
			Messages::inst().trigger_6_e14(bin_op.op,bin_op.rh->as_str());
			RETURN(error_type,true);
		}
		DeclStmt* decl = scopes.get_decl_for(trh.get_base_type(), ident_lh->ident.text);
		if (decl == nullptr)
		{
			// Error: no member named "decl->name" in trh
			Messages::inst().trigger_6_e12(bin_op.op,cpy_tlh.as_str(),decl->name.text);
			RETURN(error_type,true);
		}
		RETURN(decl->type_spec->semantic_type,true)
		// TODO: Look up member (lhs) and get it's type
	}
	return false;
}
