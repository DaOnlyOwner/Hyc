#include "LLVMBackend.h"
#include <unordered_map>
#include "llvm/IR/Constant.h"


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

		default:
			assert(false);
			break;
		}
		return true;
	}
	return false;
}

void LLVMBackendExpr::visit(BinOpExpr& bin_op)
{
	auto lhs = get(bin_op.lh);
	auto rhs = get(bin_op.rh);
	handle_pred(lhs, rhs, bin_op);
	
}
