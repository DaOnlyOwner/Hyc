#include "TypeChecker.h"
#include "TypeCreator.h"
#include "Messages.h"
#include "Token.h"
#include "GenericInstantiation.h"
#include "MemberCollector.h"
#include "ExpandScopes.h"

#define RETURN_VAL_BIN_OP(p,val) bin_op.sem_type = p; RETURN_VAL(p,val);  
#define RETURN_PREFIX_OP(p) pre_op.sem_type = p; RETURN(p); 

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

	void trigger_e6_21(const Token& name, const std::vector<FuncCallArg>& arg_list)
	{
		std::string args = "";
		if (!arg_list.empty()) args += arg_list[0].expr->sem_type.as_str();
		for (int i = 1; i < arg_list.size(); i++) args += ", " + arg_list[i].expr->sem_type.as_str();
		Messages::inst().trigger_6_e21(name, args);
	}


}

void TypeChecker::visit(DecimalLiteralExpr& lit)
{
	switch (lit.type)
	{
	case DecimalLiteralType::Float:
	{
		Type t(scopes.get_type("float"));
		lit.sem_type = t;
		RETURN(t);
	}
	case DecimalLiteralType::Double:
	{
		Type t(scopes.get_type("double"));
		lit.sem_type = t;
		RETURN(t);
	}
	case DecimalLiteralType::Quad:
	{
		Type t(scopes.get_type("quad"));
		lit.sem_type = t;
		RETURN(t);
	}
	default:
		assert(false);
	}
}

void TypeChecker::visit(IntegerLiteralExpr& lit)
{
	Type t;
	switch (lit.type)
	{
	case IntegerLiteralType::Int:
		t = Type(scopes.get_type("int"));
		break;
	case IntegerLiteralType::Half:
		t = Type(scopes.get_type("half"));
		break;
	case IntegerLiteralType::Short:
		t = Type(scopes.get_type("short"));
		break;
	case IntegerLiteralType::Char:
		t = Type(scopes.get_type("char"));
		break;
	case IntegerLiteralType::UInt:
		t = Type(scopes.get_type("uint"));
		break;
	case IntegerLiteralType::UHalf:
		t = Type(scopes.get_type("uhalf"));
		break;
	case IntegerLiteralType::UShort:
		t = Type(scopes.get_type("ushort"));
		break;
	case IntegerLiteralType::UChar:
		t = Type(scopes.get_type("uchar"));
		break;
	default:
		assert(false);
		break;
	}
	lit.sem_type = t;
	RETURN(t);
}

// TODO: Handle =: *a = 3; <-- here *a needs to be an lvalue.
// Don't allow things like i+1 = 2;
// Everything on the lhs needs to be an lvalue.
void TypeChecker::visit(BinOpExpr& bin_op)
{
	bool handled = handle_bin_op_member_acc(bin_op);
	if (!handled)
	{
		auto tlh = get(bin_op.lh);
		auto trh = get(bin_op.rh);
		handled = handled || handle_bin_op_void(tlh, trh, bin_op);
		handled = handled || handle_bin_op_predefined(tlh, trh, bin_op);
		handled = handled || handle_bin_op_pointer_arithmetic(tlh, trh, bin_op);
		handled = handled || handle_bin_op_pointer_types(tlh, trh, bin_op);
		handled = handled || handle_bin_op_inferred(tlh, trh, bin_op);
		//handled = handled || handle_bin_op_copy_move(tlh, trh, bin_op);
		if (!handled)
			handle_bin_op_overloads(tlh, trh, bin_op);
	}
}

void TypeChecker::visit(PrefixOpExpr& pre_op)
{
	Type t = get(pre_op.lh);
	if (t.must_be_inferred())
	{
		// Prefix operation: must have a type.
		Messages::inst().trigger_6_e1(pre_op.op, pre_op.lh->as_str());
		RETURN_PREFIX_OP(error_type);
	}
	if (is_pred(scopes,t))
	{
		auto pt = scopes.get_predefined_type(t.get_base_type());
		switch (pre_op.op.type)
		{
		case Token::Specifier::Plus:
			RETURN_PREFIX_OP(t);
		case Token::Specifier::Minus:
			RETURN_PREFIX_OP(t);
		case Token::Specifier::ExclMark:
		{
			if (pt != PredefinedType::Bool)
			{
				Messages::inst().trigger_6_e18(pre_op.op,t.as_str(),pre_op.lh->as_str());
				RETURN_PREFIX_OP(error_type);
			}
			Type b(scopes.get_type("bool"));
			RETURN_PREFIX_OP(b);
		}
		case Token::Specifier::Tilde:
			if (!Type::is_integer(pt))
			{
				Messages::inst().trigger_6_e6_2(pre_op.op, t.as_str());
				RETURN_PREFIX_OP(error_type);
			}
			RETURN_PREFIX_OP(t);
		default:
			break;
		}
	}
	if (pre_op.op.type == Token::Specifier::Asterix)
	{
		if (!t.is_pointer_type())
		{
			// Error: no indirection for non pointer.
			Messages::inst().trigger_6_e2(pre_op.op, pre_op.lh->as_str());
			RETURN(error_type);
		}
		t.pop();
		pre_op.sem_type = t;
		RETURN(t);
	}
	if (pre_op.op.type == Token::Specifier::Ampersand)
	{
		t.promote_pointer();
		pre_op.sem_type = t;
		RETURN(t);
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
		post_op.sem_type = error_type;
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
	// TODO: Function pointers
	auto* decl = scopes.get_variable(ident.name.text);
	if (!decl)
	{
		Messages::inst().trigger_6_e19(ident.name);
		ident.sem_type = error_type;
		RETURN(error_type);
	}
	if (decl->type_spec == nullptr && decl->type.must_be_inferred())
	{
		ident.sem_type = Type();
		RETURN(Type());
	}
	ident.sem_type = decl->type;
	RETURN(decl->type);
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	scopes.descend();
	size_t size_before_pasting = namespace_stmt.stmts.size();
	for (int i = new_elem_idx; i < size_before_pasting; i++) namespace_stmt.stmts[i]->accept(*this);
	new_elem_idx = size_before_pasting;
	scopes.go_to_root();
}

void TypeChecker::visit(FuncCallExpr& func_call_expr)
{
	auto* ident = dynamic_cast<IdentExpr*>(func_call_expr.from.get());
	
	// It's a func call expr
	if (ident)
	{

		std::for_each(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), [&](const FuncCallArg& arg) {
			arg.expr->accept(*this);
			});

		auto def = scopes.get_func(ident->name.text, [&](const FuncDeclStmt& decl) {
			if (decl.arg_list.size() != func_call_expr.arg_list.size()) return false;
			if (!decl.generic_list.empty()) return true;// If it's generic, return it because it is the only function with the specified name and argument amount in the list (this might be a canditate)
			for (int i = 0; i < decl.arg_list.size(); i++)
			{
				auto argtype_call = func_call_expr.arg_list[i].expr->sem_type;
				if (argtype_call != decl.arg_list[i]->type) return false;
			}
			return true;
			});

		if (!def)
		{
			trigger_e6_21(ident->name, func_call_expr.arg_list);
			func_call_expr.sem_type = error_type;
			RETURN(error_type);
		}

		// It's a generic function, we instantiate the function first.
		std::vector<Type> deduced_types;
		int garg_idx = 0; // The first given generic argument of the func call expr
		for (int i = 0; i < def->decl->generic_list.size(); i++)
		{
			auto& garg = def->decl->generic_list[i];
			// We have a generic type parameter garg
			// 1. Look up the idx where the generic type is used in the args 
			// 2. Look up the type of the arg at the idx in the function call expr.
			// 3. The type of the arg in the function call expr is the deduced type

			// -> 1.
			auto it = std::find_if(def->decl->arg_list.begin(), def->decl->arg_list.end(), [&](const uptr<DeclStmt>& arg) {
				return arg->type_spec->as_str() == garg.name.text;
				});

			// -> 2.
			if (it != def->decl->arg_list.end())
			{
				int arg_idx = std::distance(def->decl->arg_list.begin(), it);
				auto& deduced_type = func_call_expr.arg_list[arg_idx].expr->sem_type;
				// -> 3.
				deduced_types.push_back(deduced_type);
			}

			// Generic Type was not found in arg list
			// See if it was given
			else if (garg_idx < ident->generic_params.size())
			{
				auto& type_spec = ident->generic_params[garg_idx];
				auto [sem_type,succ] = create_type(*type_spec, scopes, ns);
				if (!succ)
				{
					Messages::inst().trigger_4_e1(type_spec->get_ident_token());
					func_call_expr.sem_type = error_type;
					RETURN(error_type);
				}
				type_spec->semantic_type = sem_type;
				deduced_types.push_back(sem_type);
			}

			else if (garg.default_type != nullptr)
			{
				auto& type_spec = garg.default_type;
				auto [sem_type, succ] = create_type(*type_spec, scopes, ns);
				if (!succ)
				{
					Messages::inst().trigger_4_e1(type_spec->get_ident_token());
					func_call_expr.sem_type = error_type;
					RETURN(error_type);
				}
				deduced_types.push_back(sem_type);
			}

			else
			{
				Messages::inst().trigger_6_e20(garg.name, func_call_expr.as_str());
				RETURN(error_type);
			}
		}

		if (!deduced_types.empty())
		{			
			std::vector<uptr<TypeSpec>> ast_deduced_types;
			std::transform(deduced_types.begin(), deduced_types.end(), std::back_inserter(ast_deduced_types), [&](const Type& t) {
				return t.to_ast();
				});

			CodePaster code_paster(*def, scopes, ast_deduced_types, ns);
			code_paster.paste();

			auto non_generic_def = scopes.get_func(get_str(def->decl->name.text, ast_deduced_types), [&](const FuncDeclStmt& decl) {
				return true; // Only one function with the new name should be in it
				});

			assert(non_generic_def);
			auto [t, succ] = create_type(*non_generic_def->decl->ret_type, scopes, ns, true);
			if (!succ)
			{
				Messages::inst().trigger_4_e1(non_generic_def->decl->ret_type->get_ident_token());
				func_call_expr.sem_type = error_type;
				RETURN(error_type);
			}

			// Make sure the args of the func call expr has the same types as the args of the selected generic function
			if (func_call_expr.arg_list.size() != non_generic_def->decl->arg_list.size())
			{
				trigger_e6_21(ident->name, func_call_expr.arg_list);
				func_call_expr.sem_type = error_type;
				RETURN(error_type);
			}

			for (int i = 0; i < func_call_expr.arg_list.size(); i++)
			{
				auto& t1 = func_call_expr.arg_list[i].expr->sem_type;
				auto& t2 = non_generic_def->decl->arg_list[i]->type;
				if (t1 != t2)
				{
					trigger_e6_21(ident->name, func_call_expr.arg_list);
					func_call_expr.sem_type = error_type;
					RETURN(error_type);
				}
			}


			func_call_expr.sem_type = t;
			func_call_expr.def = def;
			RETURN(t);
		}

		else
		{
			func_call_expr.sem_type = def->decl->ret_type->semantic_type;
			func_call_expr.def = def;
			RETURN(func_call_expr.sem_type);
		}
	}

	// It's a func ptr call (or a lambda call)
	else
	{
		// TODO Func Ptr call
		assert(false);
	}
}

void TypeChecker::visit(FuncDeclStmt& func_decl)
{
	for (auto& p : func_decl.arg_list) p->accept(*this);
}

void TypeChecker::visit(FuncDefStmt& func_def_stmt)
{
	scopes.descend();
	if (!func_def_stmt.decl->generic_list.empty()) return; // Don't try to typecheck generic functions
	func_def_stmt.decl->accept(*this);
	for(auto& p : func_def_stmt.body) p->accept(*this);
}

void TypeChecker::visit(CollectionStmt& coll_def)
{
}

void TypeChecker::visit(ReturnStmt& ret_stmt)
{
	ret_stmt.returned_expr->accept(*this);
}

void TypeChecker::visit(ExprStmt& expr_stmt)
{
	expr_stmt.expr->accept(*this);
}

void TypeChecker::visit(DeclStmt& decl_stmt)
{
	if (decl_stmt.type_spec != nullptr)
	{
		auto [t, succ] = create_type(*decl_stmt.type_spec, scopes, ns, false);
		assert(succ);
		decl_stmt.type = t;
	}
	bool succAdd = scopes.add(&decl_stmt);
	if (!succAdd)
	{
		Messages::inst().trigger_6_e17(decl_stmt.name);
	}
}

void TypeChecker::visit(ImplicitCastExpr& ice)
{
	ice.expr->accept(*this);
}

void TypeChecker::visit(IfStmt& if_stmt)
{
	scopes.descend();
	for (auto& p : if_stmt.if_stmts) p->accept(*this);
	for (auto& elif : if_stmt.all_elif_stmts)
	{
		scopes.descend();
		for (auto& p : elif) p->accept(*this);
	}
	scopes.descend();
	for (auto& p : if_stmt.else_stmts) p->accept(*this);
}

void TypeChecker::visit(WhileStmt& while_stmt)
{
	scopes.descend();
	for (auto& p : while_stmt.stmts) p->accept(*this);
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
	for (auto& case_ : match.match_cases)
	{
		scopes.descend();
		case_.decl_stmt->accept(*this);
		for (auto& p : case_.body)
		{
			p->accept(*this);
		}
	}
}

void TypeChecker::visit(ScopeStmt& sc)
{
	scopes.descend();
	for (auto& p : sc.stmts) p->accept(*this);
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
		auto& basetype = tlh.is_pointer_type() ? trh : tlh;

		if (bin_op.op.type != Token::Specifier::Plus
			&& bin_op.op.type != Token::Specifier::Minus
			&& bin_op.op.type != Token::Specifier::Equal
			&& bin_op.op.type != Token::Specifier::PlusEqual
			&& bin_op.op.type != Token::Specifier::MinusEqual)
		{
			// Only plus, minus and = += -= ops are specified for pointer arithmetic
			Messages::inst().trigger_6_e4(bin_op.op,tlh.as_str(),trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}

		if (bin_op.op.type == Token::Specifier::Equal || bin_op.op.type == Token::Specifier::Hashtag)
		{
			auto* ptr = dynamic_cast<IntegerLiteralExpr*>(base_expr.get());
			if (!ptr)
			{
				// Ptrs may only be assigned integer literals
				Messages::inst().trigger_6_e5(bin_op.op,base_expr->as_str(),basetype.as_str(),ptrtype.as_str());
				RETURN_VAL_BIN_OP(error_type, true);

			}
			if (ptr && ptr->integer_literal.text != "0")
			{
				Messages::inst().trigger_6_e5(bin_op.op,base_expr->as_str(),basetype.as_str(),ptrtype.as_str()); 
				RETURN_VAL_BIN_OP(error_type, true);

			}
		}

		RETURN_VAL_BIN_OP(ptrtype, true);
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
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.lh), trh);
				bin_op.lh = std::move(ice);
				tlh = trh;
			}
			else if (crh == ConversionType::ImplicitCasting)
			{
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), tlh);
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
				RETURN_VAL_BIN_OP(t,true);
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
					RETURN_VAL_BIN_OP(error_type,true);
				}
				RETURN_VAL_BIN_OP(trh,true);
			}

			else
			{
				RETURN_VAL_BIN_OP(tlh,true);
			}

		}
		break;
		case Token::Specifier::DoubleAmpersand:
		case Token::Specifier::DoubleOr:
		{
			if (plh == PredefinedType::Bool && prh == PredefinedType::Bool)
			{
				RETURN_VAL_BIN_OP(tlh,true);
			}
			// && and || need to have types bool as lhs and rhs
			Messages::inst().trigger_6_e7(bin_op.op,tlh.as_str(),trh.as_str());
			RETURN_VAL_BIN_OP(error_type,true);
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
			auto inner = std::make_unique<BinOpExpr>(op_inner, std::move(inner_lhs), std::move(inner_rhs));
			auto outer = BinOpExpr(Token(Token::Specifier::Equal, "="), std::move(bin_op.lh), std::move(inner));
			bin_op = std::move(outer);
			RETURN_VAL_BIN_OP(trh, true);
		}
		// For this case always take the lhs
		case Token::Specifier::Hashtag:
		case Token::Specifier::Equal:
		{
			// For e.g. a = 4 -> always take the type of the lhs and cast the rhs to the lhs.
			if (plh != prh)
			{
				// Cast the rhs to the lhs.
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), tlh);
				bin_op.rh = std::move(ice);
				RETURN_VAL_BIN_OP(tlh, true);
			}
			RETURN_VAL_BIN_OP(trh, true);
		}
		break;
		case Token::Specifier::KwAs:
			// TODO: KwAs
			assert(false);
			break;
		default:
		{
			// Error: unsupported operation type for type and type. 
			Messages::inst().trigger_6_e8(bin_op.op,tlh.as_str(),trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
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
		RETURN_VAL_BIN_OP(error_type, true);
	}
	return false;
}

bool TypeChecker::handle_bin_op_pointer_types(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	if (tlh.is_pointer_type() && trh.is_pointer_type())
	{

		if (bin_op.op.type == Token::Specifier::NotEqual
			|| bin_op.op.type == Token::Specifier::DoubleEqual)
		{
			auto bool_type = Type(scopes.get_type("bool"));
			RETURN_VAL_BIN_OP(bool_type,true);
		}

		bool succ = handle_bin_op_copy_move(tlh, trh,bin_op);
		if (!succ)
		{
			// Every operation except =,#, == and != are not allowed on pointers
			Messages::inst().trigger_6_e4(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}
		return true;
	}
	return false;
}

bool TypeChecker::handle_bin_op_copy_move(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	bool is_copy_move = (bin_op.op.type == Token::Specifier::Equal
		|| bin_op.op.type == Token::Specifier::Hashtag);
	if (is_copy_move)
	{
		if (tlh != trh)
		{
			Messages::inst().trigger_6_e16(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}
		RETURN_VAL_BIN_OP(trh, true);
	}
	return is_copy_move;
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
			RETURN_VAL_BIN_OP(error_type,true);
		}
	
		auto* ptrlh = dynamic_cast<IdentExpr*>(bin_op.lh.get());
		if (!ptrlh)
		{
			Messages::inst().trigger_6_e15(bin_op.op, bin_op.lh->as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}

		// TODO: Get the decl stmt and assign a type to it.
		DeclStmt* decl = scopes.get_variable(ptrlh->name.text);
		decl->type = trh;
		ptrlh->sem_type = trh;
		RETURN_VAL_BIN_OP(trh,true);
	}

	// When the rhs has no type yet, it's an error
	if (trh.must_be_inferred())
	{
		// Type of rhs must be known.
		Messages::inst().trigger_6_e11(bin_op.op,bin_op.rh->as_str());
		RETURN_VAL_BIN_OP(error_type,true);
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
	bin_op.sem_type = fds->decl->ret_type->semantic_type;
	ret(fds->decl->ret_type->semantic_type);
	return true;
}

bool TypeChecker::handle_bin_op_member_acc(BinOpExpr& bin_op)
{
	// . -> operators
	if (bin_op.op.type == Token::Specifier::Dot
		|| bin_op.op.type == Token::Specifier::MemAccess)
	{
		auto tlh = get(bin_op.lh);
		auto cpy_tlh = tlh;
		if (bin_op.op.type == Token::Specifier::MemAccess)
			cpy_tlh.pop();
		if (!cpy_tlh.is_base_type())
		{
			Messages::inst().trigger_6_e13(bin_op.op,cpy_tlh.as_str()); // Pointer and other types do not have members
			RETURN_VAL_BIN_OP(error_type,true);
		}
		auto* ident_rh = dynamic_cast<IdentExpr*>(bin_op.rh.get());
		if (ident_rh == nullptr)
		{
			// Error: a->b or a.b <-- b needs to be an identifier.
			Messages::inst().trigger_6_e14(bin_op.op,bin_op.rh->as_str());
			RETURN_VAL_BIN_OP(error_type,true);
		}
		DeclStmt* decl = scopes.get_decl_for(cpy_tlh.get_base_type(), ident_rh->name.text);
		if (decl == nullptr)
		{
			// Error: no member named "decl->name" in trh
			Messages::inst().trigger_6_e12(bin_op.op,cpy_tlh.as_str(),ident_rh->name.text);
			RETURN_VAL_BIN_OP(error_type,true);
		}
		ident_rh->sem_type = decl->type;
		RETURN_VAL_BIN_OP(decl->type,true)
		// TODO: Look up member (lhs) and get it's type
	}
	return false;
}

void check_type(NamespaceStmt& ns, Scopes& sc)
{
	TypeChecker tc(sc, ns);
	ns.accept(tc);
}

void check_type_repeat(NamespaceStmt& ns, Scopes& sc)
{
	TypeChecker tc(sc, ns);
	size_t n = 0;
	while (ns.stmts.size() > n)
	{
		expand_scopes(ns, sc, n);
		n = ns.stmts.size();
		ns.accept(tc);
		collect_members(ns, sc, n);
	}
}
