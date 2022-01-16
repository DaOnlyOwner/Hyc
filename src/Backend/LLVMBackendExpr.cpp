#include "LLVMBackendExpr.h"
#include <unordered_map>
#include "llvm/IR/Constant.h"
#include "Messages.h"
#include "TypeToLLVMType.h"
#include "llvm/IR/DerivedTypes.h"
#include <variant>

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
	if (bin_op.op.type == Token::Specifier::MemAccess || bin_op.op.type == Token::Specifier::Dot
		|| bin_op.op.type == Token::Specifier::MemAccComma || bin_op.op.type == Token::Specifier::DotComma)
	{
		auto lhs = get_with_params(*bin_op.lh,false);
		auto rh_expr = dynamic_cast<IdentExpr*>(bin_op.rh.get());

		UnionDeclStmt* udecl = scopes.get_union_decl_for(bin_op.lh->sem_type.get_base_type(),rh_expr->name.text);
		llvm::Value* addr;
		if (udecl)
		{
			addr = create_casted_union_field(scopes, udecl, be, lhs);
			if (should_load)
			{
				auto tagged = be.builder.CreateStructGEP(lhs, 0);
				auto ci = llvm::ConstantInt::get(llvm::Type::getInt64Ty(be.context), udecl->tagged_value.value().val);
				be.builder.CreateStore(ci, tagged);
			}
		}
		else
		{
			size_t idx = scopes.get_decl_idx_for(bin_op.lh->sem_type.get_base_type(), rh_expr->name.text);
			assert(idx != std::numeric_limits<size_t>::max());
			if (bin_op.op.type == Token::Specifier::MemAccess || bin_op.op.type == Token::Specifier::MemAccComma)
			{
				auto mapped = map_type(bin_op.lh->sem_type,scopes,be.context);
				lhs = be.builder.CreateLoad(mapped, lhs);
			}
			addr = be.builder.CreateStructGEP(lhs, idx);
		}
		if (should_load)
		{
			RETURN_WITH_TRUE(be.builder.CreateLoad(addr));
		}
		RETURN_WITH_TRUE(addr)
	}

	return false;
}

void LLVMBackendExpr::visit(DelOpExpr& del)
{
	if (del.operation)
	{
		auto fn = be.mod.getFunction(del.operation->decl->name.text);
		llvm::Value* arg[1] = { get_with_params(del.expr, false) };
		be.builder.CreateCall(fn, arg);
	}
}

bool LLVMBackendExpr::handle_union(const BinOpExpr& bin_op)
{
	if (bin_op.op.type == Token::Specifier::DoubleQM || bin_op.op.type == Token::Specifier::DoubleEM)
	{
		auto lhs = get_with_params(*bin_op.lh, false);
		auto* rh_expr = dynamic_cast<IdentExpr*>(bin_op.rh.get());
		UnionDeclStmt* udecl = scopes.get_union_decl_for(bin_op.lh->sem_type.get_base_type(),rh_expr->name.text);
		assert(udecl);
		auto ci = llvm::ConstantInt::get(llvm::Type::getInt64Ty(be.context), udecl->tagged_value.value().val);
		if (bin_op.op.type == Token::Specifier::DoubleEM)
		{
			auto tagged = be.builder.CreateStructGEP(lhs, 0);
			RETURN_WITH_TRUE(be.builder.CreateStore(ci, tagged));
		}
		else if (bin_op.op.type == Token::Specifier::DoubleQM)
		{
			auto tagged = be.builder.CreateStructGEP(lhs, 0);
			llvm::Type* t = llvm::Type::getInt64Ty(be.context);
			auto val = be.builder.CreateLoad(t, tagged);
			RETURN_WITH_TRUE(be.builder.CreateICmpEQ(val, ci));
		}
		assert(false);
	}
	return false;
}

llvm::Value* LLVMBackendExpr::hack_ret_auto_gen(const BinOpExpr& bin_op, llvm::Value*& lhs)
{
	auto pre = dynamic_cast<PrefixOpExpr*>(bin_op.lh.get());
	if (pre)
	{
		auto ident = dynamic_cast<IdentExpr*>(pre->lh.get());
		if (ident)
		{
			if (ident->decl->is_sret)
			{
				lhs = get_with_params(*ident, false);
			}
			else lhs = get_with_params(*bin_op.lh, false);
		}
		else lhs = get_with_params(*bin_op.lh, false);
	}
	else lhs = get_with_params(*bin_op.lh, false);
	return lhs;
}

bool LLVMBackendExpr::handle_assign(const BinOpExpr& bin_op)
{
	if (bin_op.op.type == Token::Specifier::Equal
		|| bin_op.op.type == Token::Specifier::Hashtag)
	{
		RETURN_WITH_TRUE(create_assign_instr(bin_op));
	}
	return false;
}

llvm::Value* LLVMBackendExpr::create_assign_instr(const BinOpExpr& bin_op)
{
	llvm::Value* lhs = nullptr;
	if (bin_op.rh->sem_type.is_user_defined(scopes)
		|| bin_op.lh->sem_type.is_user_defined(scopes))
	{
		// Hakish solution for when *ret = rhs was automatically generated.
		lhs = hack_ret_auto_gen(bin_op, lhs);
		auto rhs = get_with_params(*bin_op.rh, false);
		/*auto lhs_align = be.mod.getDataLayout().getABITypeAlignment(lhs->getType());
		auto rhs_align = be.mod.getDataLayout().getABITypeAlignment(rhs->getType());
		auto mapped = map_type(bin_op.rh->sem_type, scopes, be.context);
		auto size = be.mod.getDataLayout().getTypeStoreSizeInBits(mapped) / 8;
		llvm::MaybeAlign align(lhs_align);
		assert(lhs_align == rhs_align);
		RETURN_WITH_TRUE(be.builder.CreateMemCpy(lhs, align, rhs, align,size));*/
		return create_call_bin_op(bin_op, lhs, rhs);
	}
	else if (bin_op.rh->sem_type.is_array_type())
	{
		lhs = hack_ret_auto_gen(bin_op, lhs);
		auto rhs = get_with_params(*bin_op.rh, false);
		auto lhs_align = be.mod.getDataLayout().getABITypeAlignment(lhs->getType());
		auto rhs_align = be.mod.getDataLayout().getABITypeAlignment(rhs->getType());
		auto mapped = map_type(bin_op.rh->sem_type, scopes, be.context);
		auto size = be.mod.getDataLayout().getTypeStoreSizeInBits(mapped) / 8;
		llvm::MaybeAlign align(lhs_align);
		assert(lhs_align == rhs_align);
		return be.builder.CreateMemCpy(lhs, align, rhs, align, size);
	}
	lhs = get_with_params(*bin_op.lh, false);
	auto rhs = get_with_params(*bin_op.rh, true);
	return be.builder.CreateStore(rhs, lhs);
}

void LLVMBackendExpr::visit(DecimalLiteralExpr& lit)
{
	get_params();
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
	get_params();
	RETURN(llvm::ConstantInt::get(be.context, llvm::APInt(lit.eval_res.amount_bits, lit.eval_res.val, lit.eval_res.is_signed)));
}

void LLVMBackendExpr::visit(BinOpExpr& bin_op)
{
	auto [should_load] = get_params();
	bool handled = false;
	handled = handled || handle_member_acc(should_load, bin_op);
	handled = handled || handle_union(bin_op);
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
	auto val = scopes.get_value(ident.name.text);
	auto decl = scopes.get_variable(ident.name.text);
	if (!should_load)
	{
		RETURN(val);
	}
	else
	{
		auto mapped = map_type(ident.sem_type, scopes, be.context);
		RETURN(be.builder.CreateLoad(mapped, val, ident.name.text));
	}
}

void LLVMBackendExpr::visit(FptrIdentExpr& fptr)
{
	// TODO: check that it is only on the rhs
	auto [should_load] = get_params();
	auto def = fptr.def;
	auto mangled = mangle(*def);
	auto func = be.mod.getFunction(mangled);
	RETURN((llvm::Value*)func);
}

void LLVMBackendExpr::visit(FuncCallExpr& func_call_expr)
{
	auto* ident = dynamic_cast<IdentExpr*>(func_call_expr.from.get());
	auto [should_load] = get_params();

	if (ident)
	{
		auto llvm_maybe_fptr = scopes.get_value(ident->name.text);
		if (llvm_maybe_fptr)
		{
			auto fptr_sem = scopes.get_variable(ident->name.text);
			auto ft = get_function_type(*fptr_sem->type.get_fptr(),scopes,be.context);

			auto& t = func_call_expr.from->sem_type;
			auto mapped = map_type(t, scopes, be.context);
		
			llvm::Value* loaded = be.builder.CreateLoad(mapped,llvm_maybe_fptr, false);
			assert(loaded);
			std::pair<llvm::FunctionType*, llvm::Value*> pair = {ft,loaded};
			std::variant<llvm::FunctionCallee, std::pair<llvm::FunctionType*, llvm::Value*>> v{ pair };
			RETURN(handle_sret_func_call(create_call_inst(func_call_expr, v), func_call_expr));
		}
		auto mangled = mangle(*func_call_expr.def);
		auto func = be.mod.getFunction(mangled);
		std::variant<llvm::FunctionCallee, std::pair<llvm::FunctionType*, llvm::Value*>> v{ func };
		RETURN(handle_sret_func_call(create_call_inst(func_call_expr,v), func_call_expr));
	}

	else
	{
		auto expr = get_with_params(*func_call_expr.from,true);
		auto ft = get_function_type(*func_call_expr.from->sem_type.get_fptr(), scopes, be.context);
		std::pair<llvm::FunctionType*, llvm::Value*> pair = {ft,expr};
		std::variant<llvm::FunctionCallee, std::pair<llvm::FunctionType*, llvm::Value*>> v{ pair };
		RETURN(handle_sret_func_call(create_call_inst(func_call_expr, v), func_call_expr));
	}
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
		auto lhs = get_with_params(*bin_op.lh, true);
		auto rhs = get_with_params(*bin_op.rh, true);
		auto cpy_type = bin_op.lh->sem_type;
		cpy_type.pop(); // Get the base type
		switch (bin_op.op.type)
		{
		case Token::Specifier::Minus:
		case Token::Specifier::Plus:
			RETURN_WITH_TRUE(be.builder.CreateInBoundsGEP(map_type(cpy_type,scopes,be.context), lhs,rhs));
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

llvm::Value* LLVMBackendExpr::create_call_bin_op(const BinOpExpr& bin_op, llvm::Value* lhs, llvm::Value* rhs)
{
	auto fn = be.mod.getFunction(mangle(bin_op));
	return be.builder.CreateCall(fn, { lhs,rhs });
}


llvm::CallInst* LLVMBackendExpr::create_call_inst(FuncCallExpr& call, const std::variant<llvm::FunctionCallee,std::pair<llvm::FunctionType*,llvm::Value*>>& callee)
{
	auto def = call.def;

	std::vector<llvm::Value*> args;
	std::vector<std::pair<Type,llvm::Value*>> allocs_to_destroy;
	args.reserve(def->decl->arg_list.size());
	for (int i = 0; i < call.arg_list.size(); i++)
	{
		auto& decl = def->decl->arg_list[i].decl;
		auto& passed = call.arg_list[i].expr;
		args.push_back(
			get_with_params(passed, decl->is_passed_as_ptr ? false : true)
		);
	}

	for (int i = 0; i < call.arg_list.size(); i++)
	{
		auto& decl = def->decl->arg_list[i].decl;
		if (decl->is_passed_as_ptr)
		{
			auto base_type = decl->type.with_pop();
			auto alloc = llvm_create_alloca(be, scopes, base_type, decl->name.text);
			auto lhs = alloc;
			auto rhs = args[i];
			assert(base_type.is_user_defined(scopes) || base_type.is_array_type());
			if (base_type.is_array_type())
			{
				auto lhs_align = be.mod.getDataLayout().getABITypeAlignment(lhs->getType());
				auto rhs_align = be.mod.getDataLayout().getABITypeAlignment(rhs->getType());
				auto mapped = map_type(base_type, scopes, be.context);
				auto size = be.mod.getDataLayout().getTypeStoreSizeInBits(mapped) / 8;
				llvm::MaybeAlign align(lhs_align);
				assert(lhs_align == rhs_align);
				be.builder.CreateMemCpy(lhs, align, rhs, align, size);
				args[i] = lhs;
			}
			else if (base_type.is_user_defined(scopes))
			{
				std::string op = def->decl->arg_list[i].moved ? "#" : "=";
				auto func_name = mangle(op, decl->type, decl->type);
				auto fn = be.mod.getFunction(func_name);
				be.builder.CreateCall(fn, { lhs,rhs });
				allocs_to_destroy.push_back({ decl->type,alloc });
				args[i] = lhs;
			}
		}
	}
	llvm::CallInst* ci;
	if (std::holds_alternative<llvm::FunctionCallee>(callee))
	{
		auto& fcallee = std::get<llvm::FunctionCallee>(callee);
		ci = be.builder.CreateCall(fcallee, args);
	}
	else
	{
		auto& [ft, val] = std::get<std::pair<llvm::FunctionType*, llvm::Value*>>(callee);
		ci = be.builder.CreateCall(ft, val, args);
	}

	for (auto& alloc : allocs_to_destroy)
	{
		auto name = fmt::format("del_{}", alloc.first.as_str_for_mangling());
		auto fn = be.mod.getFunction(name);
		be.builder.CreateCall(fn, { alloc.second });
	}

	return(ci);
}

// The middleend leaves the function call (if first parameter is sret) in an invalid state and we need to fix it
// The invalid state is determined by having void as the return argument when it is acutally non void
// Fix: After the created call, create a load instruction with the passed in sret parameter.  
llvm::Value* LLVMBackendExpr::handle_sret_func_call(llvm::CallInst* callInst, FuncCallExpr& call)
{
	if (call.def->decl->is_sret)
	{
		auto sret_type = callInst->args().begin()->get()->getType();
		auto attr = llvm::Attribute::get(be.context, llvm::Attribute::AttrKind::StructRet, sret_type);
		callInst->addParamAttr(0, attr);
		auto pf = dynamic_cast<PrefixOpExpr*>(call.arg_list[0].expr.get())->lh.get();
		assert(pf);
		auto ident = dynamic_cast<IdentExpr*>(pf);
		assert(ident);
		auto sret_val = scopes.get_value(ident->name.text);
		return sret_val;
		//return be.builder.CreateLoad(sret_val);
	}

	else if (call.def->decl->named_return)
	{
		auto pf = dynamic_cast<PrefixOpExpr*>(call.arg_list[0].expr.get())->lh.get();
		assert(pf);
		auto ident = dynamic_cast<IdentExpr*>(pf);
		assert(ident);
		auto sret_val = scopes.get_value(ident->name.text);
		return sret_val;
		//return be.builder.CreateLoad(sret_val);
	}

	return callInst;
}

void LLVMBackendExpr::visit(ArraySubscriptExpr& subs)
{
	auto [should_load] = get_params();
	auto amount = get_with_params(subs.inner, true);
	llvm::Value* ep;
	if (subs.from->sem_type.is_array_type())
	{
		auto val = get_with_params(subs.from, false);
		auto ci = llvm::ConstantInt::get(be.context, llvm::APInt(32, 0, false));
		ep = be.builder.CreateInBoundsGEP(map_type(subs.from->sem_type, scopes, be.context), val, { ci,amount });
	}
	else
	{
		auto val = get_with_params(subs.from, true);
		ep = be.builder.CreateInBoundsGEP(map_type(subs.sem_type, scopes, be.context), val, amount);
	}
	if (should_load)
	{
		auto mapped = map_type(should_load ? subs.sem_type : subs.from->sem_type, scopes, be.context);
		RETURN(be.builder.CreateLoad(mapped, ep));
	}
	RETURN(ep);
}

void LLVMBackendExpr::visit(MemOpExpr& mem)
{
	llvm::Value* from;
	if(mem.mem_type.type == Token::Specifier::MemSet)
	{
	    from = get_with_params(mem.from,true);
	}
	else from = get_with_params(mem.from,false);
    auto to = get_with_params(mem.to,false);
	auto s = get_with_params(mem.size,true);
	auto& dl = be.mod.getDataLayout();
	auto from_align = dl.getABITypeAlign(from->getType());
	auto to_align = dl.getABITypeAlign(to->getType());

	switch(mem.mem_type.type)
	{
		case Token::Specifier::MemSet:
			be.builder.CreateMemSet(to,from,s,from_align);
			RETURN(to);
		case Token::Specifier::MemCpy:
			be.builder.CreateMemCpyInline(to,to_align,from,from_align,s);
			RETURN(to);
		case Token::Specifier::MemMove:
			be.builder.CreateMemMove(to,to_align,from,from_align,s);
			RETURN(to);
		default:
			assert(false);
			break;
	}
}

void LLVMBackendExpr::visit(OffsetofExpr& e)
{
	auto mapped = llvm::dyn_cast<llvm::StructType>(map_type(e.of_type,scopes,be.context));
	assert(mapped);
	size_t idx = scopes.get_decl_idx_for(e.of_type.get_base_type(),e.member.text);
	auto sl = be.mod.getDataLayout().getStructLayout(mapped);
	auto offset = sl->getElementOffsetInBits(idx);
	RETURN(llvm::ConstantInt::get(be.context, llvm::APInt(64, offset, false)));
}

void LLVMBackendExpr::visit(SizeOrAlignmentInfoExpr& e)
{
	auto mapped = map_type(e.expr->sem_type,scopes,be.context);
	auto dl = be.mod.getDataLayout();
	assert(e.op.type == Token::Specifier::KwSizeof || e.op.type == Token::Specifier::KwAlignof);
	if(e.op.type == Token::Specifier::KwSizeof)
	{
		auto size = dl.getTypeStoreSizeInBits(mapped);
		RETURN(llvm::ConstantInt::get(be.context,llvm::APInt(64,size/8,false)));
	}
	else
	{
		auto align = (uint64_t)dl.getABITypeAlignment(mapped);
		RETURN(llvm::ConstantInt::get(be.context,llvm::APInt(64,align,false)));
	}
}

void LLVMBackendExpr::visit(SizeBetweenMemberInfoExpr& e)
{
	auto mapped = llvm::dyn_cast<llvm::StructType>(map_type(Type(e.of), scopes, be.context));
	assert(mapped);
	auto sl = be.mod.getDataLayout().getStructLayout(mapped);
	auto mem1_idx = scopes.get_decl_idx_for(e.of, e.mem1.text);
	auto mem2_idx = scopes.get_decl_idx_for(e.of, e.mem2.text);
	assert(mem1_idx != std::numeric_limits<size_t>::max() && mem2_idx != std::numeric_limits<size_t>::max());
	mem2_idx++;
	uint64_t offset_to_mem2;
	if (mem2_idx >= e.of->stmts.size())
	{
		offset_to_mem2 = sl->getSizeInBits();
	}
	else offset_to_mem2 = sl->getElementOffsetInBits(mem2_idx);
	auto offset_to_mem1 = sl->getElementOffsetInBits(mem1_idx);
	auto dst = offset_to_mem2 - offset_to_mem1;
	assert(dst % 8 == 0);
	RETURN(llvm::ConstantInt::get(be.context, llvm::APInt(64, dst/8, false)));
}


void LLVMBackendExpr::visit(TernaryExpr& tern)
{
	NOT_IMPLEMENTED;
}
