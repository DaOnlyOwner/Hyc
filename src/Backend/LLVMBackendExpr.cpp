#include "LLVMBackendExpr.h"
#include <unordered_map>
#include "llvm/IR/Constant.h"
#include "Messages.h"
#include "TypeToLLVMType.h"

bool LLVMBackendExpr::handle_pred(bool should_load, const BinOpExpr& bin_op)
{
	if (bin_op.lh->sem_type.is_predefined(scopes) && bin_op.rh->sem_type.is_predefined(scopes))
	{
		auto lhs = get_with_params(*bin_op.lh, should_load);
		auto rhs = get_with_params(*bin_op.rh, should_load);
		auto plh = bin_op.lh->sem_type.to_pred(scopes).value();
		auto prh = bin_op.rh->sem_type.to_pred(scopes).value();
		assert(plh == prh);
		switch (bin_op.op.type)
		{
		case Token::Specifier::Plus:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFAdd(lhs, rhs));
			}
			else
			{
				RETURN_WITH_TRUE(be.builder.CreateAdd(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Minus:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFSub(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateSub(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Asterix:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFMul(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateMul(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Slash:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFDiv(lhs, rhs));
			}
			else if (Type::is_unsigned_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateUDiv(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateSDiv(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Ampersand:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN_WITH_TRUE(be.builder.CreateAnd(lhs, rhs));
		case Token::Specifier::Or:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN_WITH_TRUE(be.builder.CreateOr(lhs, rhs));
		case Token::Specifier::Percent:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateSRem(lhs, rhs));
			}
			else if (Type::is_unsigned_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateURem(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Caret:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN_WITH_TRUE(be.builder.CreateXor(lhs, rhs));
		case Token::Specifier::ShiftLeft:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN_WITH_TRUE(be.builder.CreateShl(lhs, rhs));
		case Token::Specifier::ShiftRight:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN_WITH_TRUE(be.builder.CreateAShr(lhs, rhs));
		case Token::Specifier::Less:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpOLT(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLT, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULT, lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Greater:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpOGT(lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGT, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGT, lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::LessEql:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpOLE(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLE, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULE, lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::GreaterEql:
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpOGE(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGE, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGE, lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::NotEqual:
		{
			auto maybe_literal = llvm::dyn_cast<llvm::Constant>(rhs);
			if (!maybe_literal)
			{
				maybe_literal = llvm::dyn_cast<llvm::Constant>(lhs);
			}
			if (maybe_literal && maybe_literal->isNullValue())
			{
				RETURN_WITH_TRUE(be.builder.CreateIsNotNull(maybe_literal == rhs ? lhs : rhs));
			}
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpONE(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, lhs, rhs));
			}
			assert(false);
		}
		break;
		case Token::Specifier::DoubleEqual:
		{
			auto maybe_literal = llvm::dyn_cast<llvm::Constant>(rhs);
			if (!maybe_literal)
			{
				maybe_literal = llvm::dyn_cast<llvm::Constant>(lhs);
			}
			if (maybe_literal && maybe_literal->isNullValue())
			{
				RETURN_WITH_TRUE(be.builder.CreateIsNull(maybe_literal == rhs ? lhs : rhs));
			}
			if (Type::is_decimal(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateFCmpOEQ(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, lhs, rhs));
			}
			assert(false);
		}
		break;
		case Token::Specifier::DoubleOr:
			assert(plh == PredefinedType::Bool);
			RETURN_WITH_TRUE(be.builder.CreateLogicalOr(lhs, rhs));
		case Token::Specifier::DoubleAmpersand:
			assert(plh == PredefinedType::Bool);
			RETURN_WITH_TRUE(be.builder.CreateLogicalAnd(lhs, rhs));
		default:
			assert(false);
			break;
		}
		return true;
	}
	return false;
}

bool LLVMBackendExpr::handle_member_acc(bool should_load, const BinOpExpr& bin_op)
{
	if (bin_op.op.type == Token::Specifier::MemAccess || bin_op.op.type == Token::Specifier::Dot)
	{
		auto lhs = get_with_params(*bin_op.lh,false);
		auto rh_expr = dynamic_cast<IdentExpr*>(bin_op.rh.get());
		size_t idx = scopes.get_decl_idx_for(bin_op.lh->sem_type.get_base_type(), rh_expr->name.text);
		assert(idx != std::numeric_limits<size_t>::max());
		auto addr = be.builder.CreateStructGEP(lhs, idx);
		if (should_load)
		{
			RETURN_WITH_TRUE(be.builder.CreateLoad(addr));
		}
		RETURN_WITH_TRUE(addr)
	}

	return false;
}

bool LLVMBackendExpr::handle_assign(const BinOpExpr& bin_op)
{
	if (bin_op.op.type == Token::Specifier::Equal)
	{
		auto lhs = get_with_params(*bin_op.lh, false);
		auto rhs = get_with_params(*bin_op.rh, true);
		RETURN_WITH_TRUE(be.builder.CreateStore(rhs, lhs));
	}
	return false;
}

void LLVMBackendExpr::visit(DecimalLiteralExpr& lit)
{
	switch (lit.type)
	{
	case DecimalLiteralType::Float:
		RETURN(llvm::ConstantFP::get(be.context, llvm::APFloat(std::get<float>(lit.decimal_holder))));
	case DecimalLiteralType::Double:
		RETURN(llvm::ConstantFP::get(be.context, llvm::APFloat(std::get<double>(lit.decimal_holder))));
	default:
		assert(false);
		break;
	}
}

void LLVMBackendExpr::visit(IntegerLiteralExpr& lit)
{
	RETURN(llvm::ConstantInt::get(be.context, llvm::APInt(lit.eval_res.amount_bits, lit.eval_res.val, lit.eval_res.is_signed)));
}

void LLVMBackendExpr::visit(BinOpExpr& bin_op)
{
	auto [should_load] = get_params();
	bool handled = false;
	handled = handled || handle_member_acc(should_load, bin_op);
	handled = handled || handle_assign(bin_op);
	handled = handled || handle_pred(should_load, bin_op);
}

void LLVMBackendExpr::visit(PrefixOpExpr& pre_op)
{
	auto [should_load] = get_params();
	if (pre_op.op.type == Token::Specifier::Ampersand)
	{
		auto e = get_with_params(*pre_op.lh, false); // Just get the address
		RETURN(e);
	}

	if (pre_op.op.type == Token::Specifier::Asterix)
	{
		auto e = get_with_params(*pre_op.lh, should_load); // Load the address
		auto mapped = map_type(should_load ? pre_op.sem_type : pre_op.lh->sem_type, scopes, be.context);
		RETURN(be.builder.CreateLoad(mapped,e));
	}

	bool is_predef = pre_op.sem_type.is_predefined(scopes);
	if (is_predef)
	{
		auto expr = get_with_params(*pre_op.lh, should_load);
		switch (pre_op.op.type)
		{
		case Token::Specifier::Plus:
			break; // Ignore
		case Token::Specifier::Minus:
			RETURN(be.builder.CreateNeg(expr));
		case Token::Specifier::ExclMark:
			RETURN(be.builder.CreateNot(expr));
		case Token::Specifier::Tilde:
			RETURN(be.builder.CreateNot(expr));
		default:
			break;
		}
	}
}

void LLVMBackendExpr::visit(PostfixOpExpr& post_op)
{
	NOT_IMPLEMENTED;
}

void LLVMBackendExpr::visit(IdentExpr& ident)
{
	// Look into allocated function variables:
	// TODO: What if we are in global scope?
	auto [should_load] = get_params();
	auto current_func_name = get_curr_fn()->getName().str();
	auto it = mem.function_stack.find(ident.name.text);
	if (it == mem.function_stack.end())
	{
		it = mem.global_mem.find(ident.name.text);
		assert(it != mem.global_mem.end());
	}
	if (!should_load)
	{
		RETURN(it->second);
	}
	else
	{
		auto mapped = map_type(ident.sem_type, scopes, be.context);
		RETURN(be.builder.CreateLoad(mapped, it->second, ident.name.text));
	}
}

void LLVMBackendExpr::visit(FuncCallExpr& func_call_expr)
{
	auto [_] = get_params();
	auto mangled = mangle(*func_call_expr.def->decl);
	auto func = be.mod.getFunction(mangled);
	std::vector<llvm::Value*> args;
	std::transform(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), std::back_inserter(args), [&](const FuncCallArg& arg) {
		return get_with_params(*arg.expr,true);
		});
	RETURN(be.builder.CreateCall(func, args));
}

void LLVMBackendExpr::visit(ImplicitCastExpr& ice)
{
	auto [sl] = get_params();
	auto from_pd_maybe = ice.expr->sem_type.to_pred(scopes);
	auto to_pd_maybe = ice.sem_type.to_pred(scopes);
	auto from = get_with_params(ice.expr,true);
	llvm::Type* to_t = map_type(ice.sem_type, scopes, be.context);
	if (from_pd_maybe.has_value() && to_pd_maybe.has_value())
	{
		llvm::Instruction::CastOps co;
		auto from_pd = from_pd_maybe.value();
		auto to_pd = to_pd_maybe.value();
		int from_w = (int)from_pd;
		int to_w = (int)to_pd;
		if (from_w + 4 == to_w) // Cast from e.g. uint to int -> not possible in llvm
			return;

		// TODO: Casts: See here: https://llvm.org/doxygen/classllvm_1_1CastInst.html
		if (Type::is_unsigned_integer(from_pd) && Type::is_decimal(to_pd))
		{
			co = llvm::Instruction::CastOps::UIToFP;
			RETURN(be.builder.CreateCast(co, from, to_t));
		}
		else if (Type::is_signed_integer(from_pd) && Type::is_decimal(to_pd))
		{
			co = llvm::Instruction::CastOps::SIToFP;
			RETURN(be.builder.CreateCast(co, from, to_t));
		}
		else if (Type::is_decimal(from_pd) && Type::is_unsigned_integer(to_pd))
		{
			co = llvm::Instruction::CastOps::FPToUI;
			RETURN(be.builder.CreateCast(co, from, to_t));
		}
		else if (Type::is_decimal(from_pd) && Type::is_signed_integer(to_pd))
		{
			co = llvm::Instruction::CastOps::FPToSI;
			RETURN(be.builder.CreateCast(co, from, to_t));
		}
		// floating point cast
		else if (Type::is_decimal(from_pd) && Type::is_decimal(to_pd))
		{
			RETURN(be.builder.CreateFPCast(from, to_t));
		}
		// Integer cast, ignore signed to unsigned and unsigned to signed (they don't exist in llvm)
		else if ((Type::is_unsigned_integer(from_pd) && Type::is_unsigned_integer(to_pd))
			|| (Type::is_signed_integer(from_pd) && Type::is_unsigned_integer(to_pd)))
		{
			RETURN(be.builder.CreateIntCast(from, to_t, false));
		}

		else if (Type::is_signed_integer(from_pd) && Type::is_signed_integer(to_pd)
			|| (Type::is_unsigned_integer(from_pd) && Type::is_signed_integer(to_pd)))
		{
			RETURN(be.builder.CreateIntCast(from, to_t, true));
		}
		else
		{
			assert(false);
			return;
		}
	}

	assert(false); // Only basic types need to be implicitly casted.
}

bool LLVMBackendExpr::handle_ptr(const BinOpExpr& bin_op)
{
	if (bin_op.lh->sem_type.is_pointer_type() && bin_op.rh->sem_type.is_predefined(scopes))
	{
		auto lhs = get_with_params(*bin_op.lh, false);
		auto rhs = get_with_params(*bin_op.rh, true);
		auto cpy_type = bin_op.lh->sem_type;
		cpy_type.pop(); // Get the base type
		switch (bin_op.op.type)
		{
		case Token::Specifier::Minus:
		case Token::Specifier::Plus:
			RETURN_WITH_TRUE(be.builder.CreateGEP(map_type(cpy_type,scopes,be.context), lhs,rhs));
		default:
			assert(false);
			break;
		}
	}

	if (bin_op.lh->sem_type.is_pointer_type() && bin_op.rh->sem_type.is_pointer_type())
	{
		auto lhs = get_with_params(*bin_op.lh, false);
		auto rhs = get_with_params(*bin_op.rh, true);
		switch (bin_op.op.type)
		{
		case Token::Specifier::Less:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULT, lhs, rhs));
			break;
		case Token::Specifier::Greater:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGT, lhs, rhs));
			break;
		case Token::Specifier::LessEql:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULE, lhs, rhs));
			break;
		case Token::Specifier::GreaterEql:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGE, lhs, rhs));
			break;
		case Token::Specifier::NotEqual:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, lhs, rhs));
			break;
		case Token::Specifier::DoubleEqual:
			RETURN_WITH_TRUE(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, lhs, rhs))
			break;
		case Token::Specifier::Minus:
			RETURN_WITH_TRUE(be.builder.CreatePtrDiff(lhs, rhs));
		}
	}
	return false;
}

void LLVMBackendExpr::visit(ArraySubscriptExpr& subs)
{
	NOT_IMPLEMENTED;
}

void LLVMBackendExpr::visit(TernaryExpr& tern)
{
	NOT_IMPLEMENTED;
}