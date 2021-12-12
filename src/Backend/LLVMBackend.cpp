#include "LLVMBackend.h"
#include <unordered_map>
#include "llvm/IR/Constant.h"

void LLVMBackendExpr::visit(BinOpExpr& bin_op)
{
	auto lhs = get(bin_op.lh);
	auto rhs = get(bin_op.rh);
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
		case Token::Specifier::Minus:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFSub(lhs, rhs));
			}
			else if(Type::is_integer(plh))
			{
				RETURN(be.builder.CreateSub(lhs, rhs));
			}
			assert(false);
		case Token::Specifier::Asterix:
			if (Type::is_decimal(plh))
			{
				RETURN(be.builder.CreateFMul(lhs, rhs));
			}
			else if (Type::is_integer(plh))
			{
				RETURN(be.builder.CreateMul(lhs, rhs));
			}
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
		default:
			break;
		}
	}
}
