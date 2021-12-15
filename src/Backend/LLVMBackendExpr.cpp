#include "LLVMBackendExpr.h"
#include <unordered_map>
#include "llvm/IR/Constant.h"
#include "Messages.h"

bool LLVMBackendExpr::handle_pred(llvm::Value* lhs, llvm::Value* rhs, const BinOpExpr& bin_op)
{
	if (bin_op.lh->sem_type.is_predefined(scopes) && bin_op.rh->sem_type.is_predefined(scopes))
	{
		auto plh = bin_op.lh->sem_type.to_pred(scopes).value();
		auto prh = bin_op.rh->sem_type.to_pred(scopes).value();
		assert(plh == prh);
		switch (bin_op.op.type)
		{
		case Token::Specifier::Plus:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFAdd(lhs, rhs));
			}
			else
			{
				RETURN(be.builder.CreateAdd(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Minus:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFSub(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN(be.builder.CreateSub(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Asterix:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFMul(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN(be.builder.CreateMul(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Slash:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFDiv(lhs, rhs));
			}
			else if (Type::is_unsigned_integer(plh))
			{
				RETURN(be.builder.CreateUDiv(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateSDiv(lhs, rhs));
			}
			assert(false);
			break;
		case Token::Specifier::Ampersand:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN(be.builder.CreateAnd(lhs, rhs));
		case Token::Specifier::Or:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN(be.builder.CreateOr(lhs, rhs));
		case Token::Specifier::Percent:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateSRem(lhs, rhs));
			}
			else if (Type::is_unsigned_integer(plh))
			{
				RETURN(be.builder.CreateURem(lhs, rhs));
			}
		case Token::Specifier::Caret:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN(be.builder.CreateXor(lhs, rhs));
		case Token::Specifier::ShiftLeft:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN(be.builder.CreateShl(lhs, rhs));
		case Token::Specifier::ShiftRight:
			assert(Type::is_integer(plh) && Type::is_integer(prh));
			RETURN(be.builder.CreateAShr(lhs, rhs));
		case Token::Specifier::Less:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpOLT(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLT, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULT, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::Greater:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpOGT(lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGT, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGT, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::LessEql:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpOLE(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLE, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_ULE, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::GreaterEql:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpOGE(lhs, rhs));
			}
			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGE, lhs, rhs));
			}

			else if (Type::is_signed_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_UGE, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::NotEqual:
			auto maybe_literal = llvm::dyn_cast<llvm::Constant>(rhs);
			if (!maybe_literal)
			{
				maybe_literal = llvm::dyn_cast<llvm::Constant>(lhs);
			}
			if (maybe_literal && maybe_literal->isNullValue())
			{
				RETURN(be.builder.CreateIsNotNull(maybe_literal == rhs ? lhs : rhs));
			}
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpONE(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::DoubleEqual:
			auto maybe_literal = llvm::dyn_cast<llvm::Constant>(rhs);
			if (!maybe_literal)
			{
				maybe_literal = llvm::dyn_cast<llvm::Constant>(lhs);
			}
			if (maybe_literal && maybe_literal->isNullValue())
			{
				RETURN(be.builder.CreateIsNull(maybe_literal == rhs ? lhs : rhs));
			}
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFCmpOEQ(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN(be.builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, lhs, rhs));
			}
			assert(false);
			return;
		case Token::Specifier::DoubleOr:
			assert(plh == PredefinedType::Bool);
			RETURN(be.builder.CreateLogicalOr(lhs, rhs));
		case Token::Specifier::DoubleAmpersand:
			assert(plh == PredefinedType::Bool);
			RETURN(be.builder.CreateLogicalAnd(lhs, rhs));

		case Token::Specifier::Hashtag:
		case Token::Specifier::Equal:
			RETURN(be.builder.CreateStore(rhs, lhs));

		default:
			assert(false);
			break;
		}
		return true;
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
	auto lhs = get(bin_op.lh);
	auto rhs = get(bin_op.rh);
	handle_pred(lhs, rhs, bin_op);
}

void LLVMBackendExpr::visit(PrefixOpExpr& pre_op)
{
	NOT_IMPLEMENTED;
}

void LLVMBackendExpr::visit(PostfixOpExpr& post_op)
{
	NOT_IMPLEMENTED;
}

void LLVMBackendExpr::visit(IdentExpr& ident)
{
	// Look into allocated function variables:
	auto& func_vars = mem[*current_func_name];
	auto it = func_vars.find(ident.name.text);
	if (it == func_vars.end())
	{
		auto& global_ns = mem["global"];
		it = mem["global"].find(ident.name.text);
		assert(it != global_ns.end());
	}
	RETURN(be.builder.CreateLoad(it->second, ident.name.text));
}

void LLVMBackendExpr::visit(FuncCallExpr& func_call_expr)
{
	auto mangled = mangle(*func_call_expr.def->decl);
	auto func = be.mod.getFunction(mangled);
	std::vector<llvm::Value*> args;
	std::transform(func_call_expr.arg_list.begin(), func_call_expr.arg_list.end(), std::back_inserter(args), [&](const FuncCallArg& arg) {
		return get(arg.expr);
		});
	be.builder.CreateCall(func, args);
}

void LLVMBackendExpr::visit(ImplicitCastExpr& ice)
{
	auto from = get(ice.expr);
	auto from_pd_maybe = ice.expr->sem_type.to_pred(scopes);
	auto to_pd_maybe = ice.sem_type.to_pred(scopes);
	if (from_pd_maybe.has_value() && to_pd_maybe.has_value())
	{
		llvm::Instruction::CastOps co;
		auto from_pd = from_pd_maybe.value();
		auto to_pd = to_pd_maybe.value();
		llvm::Type* t;
		if (Type::is_unsigned_integer(from_pd) && Type::is_decimal(to_pd))
			co = llvm::Instruction::CastOps::UIToFP;
		else if (Type::is_signed_integer(from_pd) && Type::is_decimal(to_pd))
			co = llvm::Instruction::CastOps::SIToFP;
		else if (Type::is_decimal(from_pd) && Type::is_unsigned_integer(to_pd))
			co = llvm::Instruction::CastOps::FPToUI;
		else if (Type::is_decimal(from_pd) && Type::is_signed_integer(to_pd))
			co = llvm::Instruction::CastOps::FPToSI;
		if (to_pd == PredefinedType::Float)
			t = llvm::Type::getFloatTy(be.context);
		else if (to_pd == PredefinedType::Double)
			t = llvm::Type::getDoubleTy(be.context);
		else if (to_pd == PredefinedType::Quad)
			t = llvm::Type::getFP128Ty(be.context);
		else if (to_pd == PredefinedType::UInt || to_pd == PredefinedType::Int)
			t = llvm::Type::getInt64Ty(be.context);
		else if (to_pd == PredefinedType::UHalf || to_pd == PredefinedType::Half)
			t = llvm::Type::getInt32Ty(be.context);
		else if (to_pd == PredefinedType::UShort || to_pd == PredefinedType::Short)
			t = llvm::Type::getInt16Ty(be.context);
		else if (to_pd == PredefinedType::UChar || to_pd == PredefinedType::Char)
			t = llvm::Type::getInt8Ty(be.context);
		else if (to_pd == PredefinedType::Bool)
			t = llvm::Type::getInt1Ty(be.context);
		RETURN(be.builder.CreateCast(co,from,t));
	}
	assert(false); // Only basic types need to be implicitly casted.
}

void LLVMBackendExpr::visit(ArraySubscriptExpr& subs)
{
	NOT_IMPLEMENTED;
}

void LLVMBackendExpr::visit(TernaryExpr& tern)
{
	NOT_IMPLEMENTED;
}

