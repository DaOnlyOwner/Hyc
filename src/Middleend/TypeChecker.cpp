#include "TypeChecker.h"
#include "TypeCreator.h"
#include "Messages.h"
#include "Token.h"
#include "GenericInstantiation.h"
#include "MemberCollector.h"
#include "ExpandScopes.h"
#include "Mangling.h"
#include "TaggedValueChecker.h"

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

	void trigger_e6_21(const Token& name, const std::vector<Type>& arg_list)
	{
		std::string args = "";
		if (!arg_list.empty()) args += arg_list[0].as_str();
		for (int i = 1; i < arg_list.size(); i++) args += ", " + arg_list[i].as_str();
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

// Everything on the lhs needs to be an lvalue.
void TypeChecker::visit(BinOpExpr& bin_op)
{
	bool handled = handle_bin_op_member_acc(bin_op);
	handled = handled || handle_bin_op_union(bin_op);
	if (!handled)
	{
		auto tlh = get(bin_op.lh);
		auto trh = get(bin_op.rh);
		if (tlh.is_error_type() || trh.is_error_type())
		{
			bin_op.sem_type = error_type;
			RETURN(error_type);
		}
		handled = handled || handle_bin_op_void(tlh, trh, bin_op);
		handled = handled || handle_bin_op_predefined(tlh, trh, bin_op);
		handled = handled || handle_bin_op_pointer_arithmetic(tlh, trh, bin_op);
		handled = handled || handle_bin_op_pointer_types(tlh, trh, bin_op);
		handled = handled || handle_bin_op_inferred(tlh, trh, bin_op);
		handled = handled || handle_bin_op_array(tlh, trh, bin_op);
		handled = handled || handle_bin_op_userdefined(tlh, trh, bin_op);
		if (!handled)
		{
			// Error: Operator not defined for types. 
			Messages::inst().trigger_6_e4(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN(error_type);
		}
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
	if (t.is_error_type())
	{
		pre_op.sem_type = error_type;
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

	// Error: Operator not defined for types. 
	Messages::inst().trigger_6_e3(pre_op.op,t.as_str());
	RETURN(error_type);
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

void TypeChecker::visit(DelOpExpr& del)
{
	auto t = get(del.expr);
	del.sem_type = scopes.get_type("void");
	if (t.is_user_defined(scopes))
	{
		auto op = scopes.get_op(mangle(del));
		assert(op);
		del.operation = op;
	}
	RETURN(del.sem_type);
}

void TypeChecker::visit(FptrIdentExpr& fptr)
{
	// TODO: Fptr
	for (auto& p : fptr.params)
	{
		auto [t,succ] = create_type(*p, scopes, ns);
		assert(succ);
		p->semantic_type = t;
	}

	auto def = scopes.get_func(fptr.name.text, [&](const FuncDeclStmt& decl) {
		if (decl.arg_list.size() != fptr.params.size()) return false;
		if (!decl.generic_list.empty()) return true;// If it's generic, return it because it is the only function with the specified name and argument amount in the list (this might be a canditate)
		for (int i = 0; i < decl.arg_list.size(); i++)
		{
			auto argtype_call = fptr.params[i]->semantic_type;
			if (argtype_call != decl.arg_list[i].decl->type) return false;
		}
		return true;
		});

	std::vector<Type> tv;
	std::transform(fptr.params.begin(), fptr.params.end(), std::back_inserter(tv), [&](const auto& t) {
		return t->semantic_type;
		});
	
	if (!def)
	{
		Messages::inst().trigger_6_e32(fptr.name, fptr.as_str());
		fptr.sem_type = error_type;
		RETURN(error_type);
	}
	
	auto [ded_types, succ] = deduce_template_args(def, tv, fptr.generic_params, fptr);
	if (!succ)
	{
		fptr.sem_type = error_type;
		RETURN(error_type);
	}

	auto non_generic_def = paste_new_function(def, ded_types, fptr.name, tv, fptr);
	if (non_generic_def)
	{
		fptr.def = non_generic_def;
		fptr.sem_type = create_fptr_type(tv, non_generic_def->decl->ret_type->semantic_type);
		RETURN(fptr.sem_type);
	}
	else
	{
		RETURN(error_type);
	}
}

void TypeChecker::visit(IdentExpr& ident)
{
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
	ident.decl = decl;
	RETURN(decl->type);
}

void TypeChecker::visit(NamespaceStmt& namespace_stmt)
{
	size_t size_before_pasting = namespace_stmt.stmts.size();
	for (int i = new_elem_idx; i < size_before_pasting; i++) namespace_stmt.stmts[i]->accept(*this);
	new_elem_idx = size_before_pasting;
}

std::pair<std::vector<Type>,bool> TypeChecker::deduce_template_args(FuncDefStmt* def, const std::vector<Type>& params, const std::vector<uptr<TypeSpec>>& gparams, Expr& expr)
{
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
		auto it = std::find_if(def->decl->arg_list.begin(), def->decl->arg_list.end(), [&](const FuncArg& arg) {
			return arg.decl->type_spec->as_str() == garg.name.text;
			});

		// -> 2.
		if (it != def->decl->arg_list.end())
		{
			int arg_idx = std::distance(def->decl->arg_list.begin(), it);
			auto& deduced_type = params[arg_idx];
			// -> 3.
			deduced_types.push_back(deduced_type);
		}

		// Generic Type was not found in arg list
		// See if it was given
		else if (garg_idx < gparams.size())
		{
			auto& type_spec = gparams[garg_idx];
			auto [sem_type, succ] = create_type(*type_spec, scopes, ns);
			if (!succ)
			{
				Messages::inst().trigger_4_e1(type_spec->get_ident_token());
				expr.sem_type = error_type;
				return { {},false };
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
				expr.sem_type = error_type;
				return { {},false };
			}
			deduced_types.push_back(sem_type);
		}

		else
		{
			Messages::inst().trigger_6_e20(garg.name, expr.as_str());
			return { {},false };
		}
	}
	return { deduced_types,true };
}

FuncDefStmt* TypeChecker::paste_new_function(FuncDefStmt* def, const std::vector<Type>& deduced_types, const Token& name, const std::vector<Type>& params, Expr& expr)
{
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
			expr.sem_type = error_type;
			return nullptr;
		}

		// Make sure the args of the func call expr has the same types as the args of the selected generic function
		if (params.size() != non_generic_def->decl->arg_list.size())
		{
			trigger_e6_21(name, params);
			expr.sem_type = error_type;
			return nullptr;
		}

		for (int i = 0; i < params.size(); i++)
		{
			auto& t1 = params[i];
			auto& t2 = non_generic_def->decl->arg_list[i].decl->type;
			if (t1 != t2)
			{
				trigger_e6_21(name, params);
				expr.sem_type = error_type;
				return nullptr;
			}
		}


		expr.sem_type = t;
		return non_generic_def;
	}

	else
	{
		expr.sem_type = def->decl->ret_type->semantic_type;
		return def;
	}
}

void TypeChecker::handle_ident_call_expr(FuncCallExpr& func_call_expr, IdentExpr* ident)
{
	// First look up the name in the variables, maybe it is a function pointer.
	auto decl = scopes.get_variable(ident->name.text);
	if (decl && ident->generic_params.empty())
	{
		if (!decl->type.is_fptr_type())
		{
			// Function call needs to be either a function or a function pointer
			Messages::inst().trigger_6_e30(ident->name);
			func_call_expr.sem_type = error_type;
			RETURN(error_type);
		}

		auto fptr = decl->type.get_fptr();
		std::vector<Type> params;
		std::transform(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), std::back_inserter(params), [&](const FuncCallArg& p)
			{
				return p.expr->sem_type;
			});
		if (fptr->args.size() != func_call_expr.arg_list.size())
		{
			trigger_e6_21(ident->name, params);
			func_call_expr.sem_type = error_type;
			RETURN(error_type);
		}

		for (int i = 0; i < params.size(); i++)
		{
			auto& t1 = params[i];
			auto& t2 = *fptr->args[i];
			if (t1 != t2)
			{
				trigger_e6_21(ident->name, params);
				func_call_expr.sem_type = error_type;
				RETURN(error_type);
			}
		}

		func_call_expr.sem_type = *decl->type.get_fptr()->return_type;
		ident->sem_type = decl->type;
		RETURN(func_call_expr.sem_type);
	}

	auto def = scopes.get_func(ident->name.text, [&](const FuncDeclStmt& decl) {
		if (decl.arg_list.size() != func_call_expr.arg_list.size()) return false;
		if (!decl.generic_list.empty()) return true;// If it's generic, return it because it is the only function with the specified name and argument amount in the list (this might be a canditate)
		for (int i = 0; i < decl.arg_list.size(); i++)
		{
			auto argtype_call = func_call_expr.arg_list[i].expr->sem_type;
			if (argtype_call != decl.arg_list[i].decl->type) return false;
		}
		return true;
		});

	
	if (!def)
	{
		std::vector<Type> params;
		std::transform(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), std::back_inserter(params), [&](const FuncCallArg& fca) {
			return fca.expr->sem_type;
			});
		trigger_e6_21(ident->name, params);
		func_call_expr.sem_type = error_type;
		RETURN(error_type);
	}

	std::vector<Type> params;
	std::transform(def->decl->arg_list.begin(), def->decl->arg_list.end(), std::back_inserter(params), [&](const FuncArg& param) {
		return param.decl->type;
		});
	auto [deduced_types, succ] = deduce_template_args(def, params, ident->generic_params, *ident);

	if (!succ)
	{
		func_call_expr.sem_type = error_type;
		RETURN(error_type);
	}

	auto non_generic_def = paste_new_function(def, deduced_types, ident->name, params, func_call_expr);
	if (non_generic_def)
	{
		func_call_expr.def = non_generic_def;
		ident->sem_type = create_fptr_type(params, non_generic_def->decl->ret_type->semantic_type);
		RETURN(func_call_expr.sem_type);
	}
	else
	{
		RETURN(error_type);
	}

}

// TODO: Implicit cast for predefined types
void TypeChecker::visit(FuncCallExpr& func_call_expr)
{
	auto* ident = dynamic_cast<IdentExpr*>(func_call_expr.from.get());
	
	std::for_each(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), [&](const FuncCallArg& arg) {
		arg.expr->accept(*this);
		});
	// It's a func call expr
	if (ident)
	{
		handle_ident_call_expr(func_call_expr, ident);
	}

	// It's a func ptr call (or a lambda call)
	else
	{
		// TODO: Func Ptr call
		std::vector<Type> params;
		std::transform(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), std::back_inserter(params), [&](const FuncCallArg& fca) {
			return fca.expr->sem_type;
			});
		auto t = get(func_call_expr.from);
		if (!t.is_fptr_type())
		{
			Messages::inst().trigger_6_e31(func_call_expr.from->first_token(), func_call_expr.from->as_str());
			func_call_expr.sem_type = error_type;
			RETURN(error_type);
		}
		auto fptr = t.get_fptr();
		if (fptr->args.size() != func_call_expr.arg_list.size())
		{
			trigger_e6_21(func_call_expr.from->first_token(), params);
			func_call_expr.sem_type = error_type;
			RETURN(error_type);
		}

		for (int i = 0; i < params.size(); i++)
		{
			auto& t1 = params[i];
			auto& t2 = *fptr->args[i];
			if (t1 != t2)
			{
				trigger_e6_21(func_call_expr.from->first_token(), params);
				func_call_expr.sem_type = error_type;
				RETURN(error_type);
			}
		}
		assert(fptr);
		func_call_expr.sem_type = *fptr->return_type;
		RETURN(func_call_expr.sem_type);
	}
}

void TypeChecker::visit(FuncDeclStmt& func_decl)
{
	if (func_decl.named_return)
	{
		auto passed_t = func_decl.ret_type->semantic_type.with_pointer();
		func_decl.named_return->type = passed_t; 
		scopes.add(func_decl.named_return.get());
	}
	for (auto& p : func_decl.arg_list) p.decl->accept(*this);
}

void TypeChecker::visit(FuncDefStmt& func_def_stmt)
{
	if (!func_def_stmt.decl->generic_list.empty()) return; // Don't try to typecheck generic functions
	scopes.descend();
	current_function = &func_def_stmt;
	func_def_stmt.decl->accept(*this);
	bool ret_was_there = false;
	for (auto& p : func_def_stmt.body)
	{
		p->accept(*this);
		if ((ret_was_there = p->is_return_stmt())) break;
	}
	auto pred = func_def_stmt.decl->ret_type->semantic_type.to_pred(scopes);
	if (pred.has_value() && (pred.value() != PredefinedType::Void) && !ret_was_there)
	{
		Messages::inst().trigger_6_w3(func_def_stmt.decl->name);
	}
	scopes.ascend();
}

void TypeChecker::visit(TypeDefStmt& coll_def)
{
}

void TypeChecker::visit(ReturnStmt& ret_stmt)
{
	Type type;
	if (ret_stmt.returned_expr)
		type = get(ret_stmt.returned_expr);
	else type = Type(scopes.get_type("void"));
	auto pred_expr_ = type.to_pred(scopes);
	auto pred_func_ = current_function->decl->ret_type->semantic_type.to_pred(scopes);
	if (pred_expr_.has_value() && pred_func_.has_value() && pred_expr_ != PredefinedType::Void && pred_func_ != PredefinedType::Void)
	{
		auto pred_expr = pred_expr_.value();
		auto pred_func = pred_func_.value();
		if (Type::is_signed_integer(pred_expr) && Type::is_unsigned_integer(pred_func))
		{
			// Signed op unsigned warning: cast to signed
			Messages::inst().trigger_6_w1(ret_stmt.return_kw, ret_stmt.returned_expr->as_str());
		}
		else if (Type::is_unsigned_integer(pred_expr) && Type::is_signed_integer(pred_func))
		{
			Messages::inst().trigger_6_w2(ret_stmt.return_kw, ret_stmt.returned_expr->as_str());
		}
		if (pred_expr != pred_func)
		{
			uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(ret_stmt.returned_expr), current_function->decl->ret_type->semantic_type);
			ret_stmt.returned_expr = std::move(ice);
		}
		return;
	}

	auto& t_func = current_function->decl->ret_type->semantic_type;
	if (type != t_func && (current_function->decl->named_return && pred_expr_.has_value() && (pred_expr_.value() != PredefinedType::Void)))
	{
		Messages::inst().trigger_6_e25(ret_stmt.returned_expr ? ret_stmt.returned_expr->first_token() : ret_stmt.return_kw, type.as_str(), t_func.as_str());
	}
}

void TypeChecker::visit(ExprStmt& expr_stmt)
{
	expr_stmt.expr->accept(*this);
}

void TypeChecker::visit(DeclStmt& decl_stmt)
{
	if (decl_stmt.type_spec != nullptr)
	{
		auto [t, succ] = create_type_with_msg(*decl_stmt.type_spec);
		if (!succ)
		{
			decl_stmt.type = error_type;
		}
		else decl_stmt.type = t;
	}
	// Type needs to be inferred.
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
	// TODO: nullptr needs to be a special type 
	// TODO: Check in a separate file that all code paths return a value.
	// E.g. t.is_nullptr() -> returns true if type t is nullptr.
	check_type_is_bool(if_stmt.if_expr);
	scopes.descend();
	for (auto& p : if_stmt.if_stmts) p->accept(*this);
	scopes.ascend(); 
	for (int i = 0; i<if_stmt.all_elif_stmts.size(); i++)
	{
		auto& elif = if_stmt.all_elif_stmts[i];
		auto& elif_expr = if_stmt.elif_exprs[i];
		check_type_is_bool(elif_expr);
		scopes.descend();
		for (auto& p : elif)
		{
			p->accept(*this);
			if (p->is_return_stmt()) break;
		}
		scopes.ascend();
	}
	if (!if_stmt.else_stmts.empty())
	{
		scopes.descend();
		for (auto& p : if_stmt.else_stmts)
		{
			p->accept(*this);
			if (p->is_return_stmt()) break;
		}
		scopes.ascend();
	}
}

void TypeChecker::visit(WhileStmt& while_stmt)
{
	check_type_is_bool(while_stmt.expr);
	scopes.descend();
	for (auto& p : while_stmt.stmts)
	{
		p->accept(*this);
		if (p->is_return_stmt()) break;
	}
	scopes.ascend();
}

void TypeChecker::visit(ContinueStmt& cont_stmt)
{
	NOT_IMPLEMENTED;
}

void TypeChecker::visit(ArraySubscriptExpr& subs)
{
	auto t = get(subs.from);
	auto t_inner = get(subs.inner);
	auto t_inner_p = t_inner.to_pred(scopes);
	if (t_inner_p.has_value() && Type::is_integer(t_inner_p.value()))
	{
		if (!t.is_pointer_type() && !t.is_array_type())
		{
			Messages::inst().trigger_6_e26(subs.inner->first_token(), subs.from->as_str());
			subs.sem_type = error_type;
			RETURN(error_type);
		}
		t.pop();
		subs.sem_type = t;
		RETURN(t);
	}

	Messages::inst().trigger_6_e27(subs.inner->first_token(),subs.inner->as_str());
	subs.sem_type = error_type;
	RETURN(error_type);
}

void TypeChecker::visit(TernaryExpr& tern)
{
	NOT_IMPLEMENTED;
}

void TypeChecker::visit(MatchStmt& match)
{
	auto t = get(match.match_on);
	if (!t.is_base_type() || t.get_base_type()->ct != CollectionType::Union)
	{
		Messages::inst().trigger_6_e28(match.match_on->first_token(), t.as_str());
	}
	auto current_union_var = t.get_base_type();
	if (current_union_var->stmts.size() != match.match_cases.size())
	{
		for (auto& p : current_union_var->stmts)
		{
			auto udecl = dynamic_cast<UnionDeclStmt*>(p.get());
			auto it = std::find_if(match.match_cases.begin(), match.match_cases.end(), [&](const MatchCase& mc) {
				return udecl->decl_stmt->name.text == mc.var.text;
				});
			if (it == match.match_cases.end())
			{
				Messages::inst().trigger_6_w3(match.match_on->first_token(),udecl->decl_stmt->name.text);
			}
		}
	}
	for (auto& case_ : match.match_cases)
	{
		scopes.descend();
		auto* udecl = scopes.get_union_decl_for(current_union_var, case_.var.text);
		if (!udecl)
		{		
			// Error: no member named "decl->name" in trh
			Messages::inst().trigger_6_e12(case_.var, current_union_var->name.text, case_.var.text);
		}
		else
		{
			auto t2 = udecl->decl_stmt->type;
			auto pd = t2.to_pred(scopes);
			if (!(pd.has_value() && pd.value() == PredefinedType::Void))
			{
				if (case_.as.has_value())
				{
					t2.promote_pointer();
					uptr<DeclStmt> decl = std::make_unique<DeclStmt>(nullptr, case_.as.value());
					case_.as_decl = mv(decl);
					case_.as_decl->type = t2;
					bool succ = scopes.add(case_.as_decl.get());
					if (!succ)
					{
						// Already defined in this scope
						Messages::inst().trigger_6_e17(case_.as_decl->name);
					}
				}
			}
			else
			{
				if (case_.as.has_value())
				{
					Messages::inst().trigger_6_e29(case_.as.value(), case_.var.text, t.as_str());
				}
			}
		}
		for (auto& p : case_.body)
		{
			p->accept(*this);
		}
		scopes.ascend();
	}
}

void TypeChecker::visit(ScopeStmt& sc)
{
	scopes.descend();
	for (auto& p : sc.stmts)
	{
		p->accept(*this);
		if (p->is_return_stmt()) break;
	}
	scopes.ascend();
}

void TypeChecker::visit(SizeOrAlignmentInfoExpr& e)
{
	get(e.expr);
	e.sem_type = scopes.get_type("uint");
	RETURN(e.sem_type);
}

void TypeChecker::visit(OffsetofExpr& e)
{
	auto uint_type = scopes.get_type("uint");
	e.sem_type = uint_type;
	auto [t,succ] = create_type(*e.of,scopes,ns,true);
	if(!succ)
	{
		Messages::inst().trigger_4_e1(e.of->get_ident_token());
		RETURN(uint_type);
	}
	if(!t.is_user_defined(scopes))
	{
		Messages::inst().trigger_6_e35(e.of->get_ident_token(),t.as_str());
		RETURN(uint_type);
	}
	e.of_type = t;
	auto member = scopes.get_decl_for(t.get_base_type(),e.member.text);
	if(!member)
	{
		Messages::inst().trigger_6_e12(e.member,e.of->as_str(), e.member.text);
	}
	RETURN(uint_type);
}

void TypeChecker::visit(SizeBetweenMemberInfoExpr& e)
{
	e.sem_type = scopes.get_type("uint");
	RETURN(e.sem_type);
}

void TypeChecker::visit(InitListArrayExpr& init_array)
{
	Type deduced;
	int start_idx;
	if (init_array.type_to_init)
	{
		auto [created, succ] = create_type_with_msg(*init_array.type_to_init);
		if (!succ)
		{
			init_array.sem_type = error_type;
			RETURN(error_type);
		}
		if (!created.is_array_type())
		{
			//Error: Needs to be an array type
			Messages::inst().trigger_6_e39(init_array.first_token());
			init_array.sem_type = error_type;
			RETURN(error_type);
		}
		deduced = created;	
		start_idx = 0;
		if (init_array.values.empty())
		{
			init_array.sem_type = created;
			RETURN(created);
		}
	}
	else
	{
		if (init_array.values.empty())
		{
			// Cannot deduce type
			Messages::inst().trigger_6_e37(init_array.first_token());
			init_array.sem_type = error_type;
			RETURN(error_type);
		}
		auto& fst = init_array.values[0];
		deduced = get(fst);
		start_idx = 1;
	}
	for (int i = start_idx;i< init_array.values.size();i++)
	{
		auto& elem = init_array.values[i];
		auto t_further = get(elem);
		auto [succ,new_elem] = maybe_make_implicit_cast(t_further, deduced.with_pop(), std::move(elem));
		if (!succ)
		{
			// Error: deduced type t_first doesn't match t_further
			Messages::inst().trigger_6_e36(init_array.values[i]->first_token(), deduced.with_pop().as_str(), t_further.as_str());
		}
		init_array.values[i] = std::move(new_elem);
	}
	init_array.sem_type = deduced;
	RETURN(deduced);
}

void TypeChecker::visit(InitListStructExpr& init_struct)
{
	Type* deduced = nullptr;
	Type given;
	if (init_struct.type_to_init)
	{
		auto [tmp, succ] = create_type_with_msg(*init_struct.type_to_init);
		if (!succ)
		{
			init_struct.sem_type = error_type;
			RETURN(error_type);
		}
		given = tmp;
		deduced = &given;
	}
	if (current_type)
	{
		if (init_struct.type_to_init && *deduced != *current_type)
		{
			//ERROR: given type does not match the inferred type
			Messages::inst().trigger_6_e36(init_struct.first_token(),current_type->as_str(),deduced->as_str());
			init_struct.sem_type = error_type;
			RETURN(error_type);
		}
		else if (!init_struct.type_to_init) deduced = current_type;		
	}
	else
	{
		if (!init_struct.type_to_init)
		{
			// Error: Cannot infer struct type 
			Messages::inst().trigger_6_e37(init_struct.first_token());
		}
	}
	if (!init_struct.struct_init_info.empty() && !deduced->is_base_type())
	{
		Messages::inst().trigger_6_e13(init_struct.struct_init_info[0].member, given.as_str()); // Pointer and other types do not have members
		init_struct.sem_type = error_type;
		RETURN(error_type);
	}


	if (init_struct.struct_init_info.size() > 1)
	{
		if (deduced->get_base_type()->get_ct() == CollectionType::Union)
		{
			// More than one member initializer for union for initializer list.
			Messages::inst().trigger_6_e38(init_struct.first_token());
		}
	}

	for (auto& p : init_struct.struct_init_info)
	{
		DeclStmt* decl = scopes.get_decl_for(deduced->get_base_type(), p.member.text);
		Type* inner = nullptr;
		if (decl == nullptr)
		{
			auto* udecl = scopes.get_union_decl_for(deduced->get_base_type(), p.member.text);
			if (!udecl)
			{
				// Error: no member named "decl->name" in trh
				Messages::inst().trigger_6_e12(p.member, deduced->as_str(), p.member.text);
				init_struct.sem_type = error_type;
				RETURN(error_type);
			}
			inner = &udecl->decl_stmt->type;
		}
		else inner = &decl->type;
		current_type = inner;
		auto out_t = get(p.init);
	}
	init_struct.sem_type = *deduced;
	RETURN(*deduced);
}

void TypeChecker::visit(MemOpExpr& mem)
{
	auto from_t = get(mem.from);
	auto to_t = get(mem.to);
	auto size_t = get(mem.size);
	if (!from_t.is_pointer_type())
	{
		Messages::inst().trigger_6_e33(mem.from->first_token(), "source", from_t.as_str());
	}
	if (mem.mem_type.type == Token::Specifier::MemSet)
	{
		RETURN(to_t);
	}
	if (!to_t.is_pointer_type())
	{
		Messages::inst().trigger_6_e33(mem.to->first_token(), "destination", to_t.as_str());
	}

	auto spred = size_t.to_pred(scopes);
	if (!spred.has_value() || spred.value() == PredefinedType::Void)
	{
		Messages::inst().trigger_6_e34(mem.size->first_token(), size_t.as_str());
	}

	auto [succ,casted] = maybe_make_implicit_cast(size_t, Type(scopes.get_type("uint")), std::move(mem.size));
	assert(succ);
	mem.size = std::move(casted);
}

bool TypeChecker::handle_bin_op_pointer_arithmetic(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	// Pointer arithmetic
	bool is_pointer_arith = (tlh.is_pointer_type() && trh.is_predefined(scopes));

	if (is_pointer_arith)
	{
		auto& ptrtype = tlh.is_pointer_type() ? tlh : trh;
		auto& base_expr = tlh.is_base_type() ? bin_op.lh : bin_op.rh;
		auto& basetype = tlh.is_pointer_type() ? trh : tlh;
		auto pred = basetype.to_pred(scopes).value();

		if (Type::is_decimal(pred))
		{
			Messages::inst().trigger_6_e4(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN_VAL_BIN_OP(error_type,true);
		}

		if (bin_op.op.type != Token::Specifier::Plus
			&& bin_op.op.type != Token::Specifier::Minus
			&& bin_op.op.type != Token::Specifier::Equal)
		{
			// Only plus, minus and = += -= ops are specified for pointer arithmetic
			Messages::inst().trigger_6_e4(bin_op.op,tlh.as_str(),trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}

		if (bin_op.op.type == Token::Specifier::Equal)
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
		// For this case always take the lhs
		case Token::Specifier::Equal:
		{
			// For e.g. a = 4 -> always take the type of the lhs and cast the rhs to the lhs.
			if (plh != prh)
			{
				// Cast the rhs to the lhs.
				uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(bin_op.rh), tlh);
				bin_op.rh = std::move(ice);
			}
			RETURN_VAL_BIN_OP(scopes.get_type("void"), true);
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
			|| bin_op.op.type == Token::Specifier::DoubleEqual
			|| bin_op.op.type == Token::Specifier::Less
			|| bin_op.op.type == Token::Specifier::Greater
			|| bin_op.op.type == Token::Specifier::LessEql
			|| bin_op.op.type == Token::Specifier::GreaterEql)
		{
			auto bool_type = Type(scopes.get_type("bool"));
			RETURN_VAL_BIN_OP(bool_type,true);
		}

		bool succ = handle_bin_op_copy(tlh, trh,bin_op);
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

bool TypeChecker::handle_bin_op_userdefined(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	auto def = scopes.get_op(mangle(bin_op));
	if (!def)
	{
		RETURN_VAL_BIN_OP(error_type, false);
	}
	else
	{
		bin_op.sem_bin_op = def;
		RETURN_VAL_BIN_OP(def->decl->ret_type->semantic_type,true);
	}
}

bool TypeChecker::handle_bin_op_copy(Type& tlh, Type& trh, BinOpExpr& bin_op)
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
		RETURN_VAL_BIN_OP(scopes.get_type("void"), true);
	}
	return is_copy_move;
}

bool TypeChecker::handle_bin_op_array(Type& tlh, Type& trh, BinOpExpr& bin_op)
{
	if (tlh.is_array_type())
	{
		switch (bin_op.op.type)
		{
		case Token::Specifier::Equal:
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
			Messages::inst().trigger_6_e16(bin_op.op, tlh.as_str(), trh.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		default:
			break;
		}
	}
	return false;
}

bool TypeChecker::handle_bin_op_union(BinOpExpr& bin_op)
{
	// This is almost the same code as in handle_bin_op_mem_access.
	if (bin_op.op.type == Token::Specifier::DoubleEM
		|| bin_op.op.type == Token::Specifier::DoubleQM)
	{
		auto t = get(bin_op.lh);
		if (t.is_error_type())
		{
			RETURN_VAL_BIN_OP(error_type, true);
		}
		if (!t.is_base_type())
		{
			Messages::inst().trigger_6_e13(bin_op.op, t.as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}
		auto* ident_rh = dynamic_cast<IdentExpr*>(bin_op.rh.get());
		if (ident_rh == nullptr)
		{
			// Error: a!!b or a??b <-- b needs to be an identifier.
			Messages::inst().trigger_6_e14(bin_op.op, bin_op.rh->as_str());
			RETURN_VAL_BIN_OP(error_type, true);
		}
		UnionDeclStmt* decl = scopes.get_union_decl_for(t.get_base_type(), ident_rh->name.text);
		if (decl == nullptr)
		{
			// Error: no member named "decl->name" in trh
			Messages::inst().trigger_6_e12(bin_op.op, t.as_str(), ident_rh->name.text);
			ident_rh->sem_type = error_type;
			RETURN_VAL_BIN_OP(error_type, true);
		}
		ident_rh->sem_type = decl->decl_stmt->type;
		if (bin_op.op.type == Token::Specifier::DoubleQM)
		{
			RETURN_VAL_BIN_OP(Type(scopes.get_type("bool")), true);
		}
		else
		{
			RETURN_VAL_BIN_OP(Type(scopes.get_type("void")), true);
		}
	}
	return false;
}

void TypeChecker::check_type_is_bool(uptr<Expr>& expr)
{
	auto if_t = get(expr);
	auto maybe_pred = if_t.to_pred(scopes);
	if (maybe_pred.has_value())
	{
		auto pred = maybe_pred.value();
		if (pred != PredefinedType::Bool)
		{
			auto ile = std::make_unique<IntegerLiteralExpr>();
			auto cmp = std::make_unique<BinOpExpr>(Token(Token::Specifier::NotEqual, "!="), mv(expr), mv(ile));
			expr = mv(cmp);
		}
	}
	else
	{
		Messages::inst().trigger_6_e22(expr->first_token(), expr->as_str(), if_t.as_str());
	}
}

std::pair<bool, uptr<Expr>> TypeChecker::maybe_make_implicit_cast(const Type& from, const Type& to, uptr<Expr>&& child)
{
	if (from == to) return { true,std::move(child) };
	auto pre_from_ = from.to_pred(scopes);
	auto pre_to_ = to.to_pred(scopes);
	if (pre_from_.has_value() && pre_to_.has_value())
	{
		auto pre_from = pre_from_.value();
		auto pre_to = pre_to_.value();
		uptr<ImplicitCastExpr> ice = std::make_unique<ImplicitCastExpr>(std::move(child), to);
		return { true,std::move(ice) };
	}
	return {false,std::move(child)};
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

bool TypeChecker::handle_bin_op_member_acc(BinOpExpr& bin_op)
{
	// . -> operators
	if (bin_op.op.type == Token::Specifier::Dot
		|| bin_op.op.type == Token::Specifier::MemAccess
		|| bin_op.op.type == Token::Specifier::DotComma
		|| bin_op.op.type == Token::Specifier::MemAccComma)
	{
		auto tlh = get(bin_op.lh);
		if (tlh.is_error_type())
		{
			RETURN_VAL_BIN_OP(error_type, true);
		}
		auto cpy_tlh = tlh;
		if (bin_op.op.type == Token::Specifier::MemAccess || bin_op.op.type == Token::Specifier::MemAccComma)
		{
			if (!cpy_tlh.is_pointer_type())
			{
				Messages::inst().trigger_6_e23(bin_op.op, bin_op.lh->as_str(), cpy_tlh.as_str());
				RETURN_VAL_BIN_OP(error_type, true);
			}
			cpy_tlh.pop();
		}
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
			auto* udecl = scopes.get_union_decl_for(cpy_tlh.get_base_type(), ident_rh->name.text);
			if (!udecl)
			{
				// Error: no member named "decl->name" in trh
				Messages::inst().trigger_6_e12(bin_op.op, cpy_tlh.as_str(), ident_rh->name.text);
				RETURN_VAL_BIN_OP(error_type, true);
			}
			ident_rh->sem_type = udecl->decl_stmt->type;
			RETURN_VAL_BIN_OP(ident_rh->sem_type, true);
		}
		ident_rh->sem_type = decl->type;
		RETURN_VAL_BIN_OP(decl->type, true);
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
		// Collect the members of the new structs (generic instantiation is done when pasting the new function)
		instantiate_generic_repeat(ns, sc, n);
		collect_members(ns, sc, n); 
		check_tagged_values(ns, n);		
	}
}
