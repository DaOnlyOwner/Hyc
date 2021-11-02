#if 0
#include "CodeGen.h"
#include "SymbolTable.h"

#ifndef NDEBUG
#define DID(str) str
#elif
#define DID(str) ""
#endif



CodeGen::CodeGen()
{
}

void CodeGen::visit(FloatLiteralExpr& lit)
{
}

void CodeGen::visit(IntegerLiteralExpr& lit)
{
    ret(llvm::ConstantInt::get(m_context, llvm::APInt(lit.bits, lit.val)));
	return;
}

void CodeGen::visit(BinOpExpr& bin_op)
{
	llvm::Value* lh = get(bin_op.lh);
	llvm::Value* rh = get(bin_op.rh);
	
	bin_op

	auto op_type = BinaryOperator::from_token_specifier(bin_op.op.type);
	switch (op_type)
	{
	case BinaryOperator::Specifier::Plus:
		ret(m_ir_builder.CreateAdd(lh, rh, DID("op_add")));
		break;
	case BinaryOperator::Specifier::Minus:
		ret(m_ir_builder.CreateSub(lh, rh, DID("op_sub")));
		break;
	case BinaryOperator::Specifier::Multiplication:
		ret(m_ir_builder.CreateMul(lh, rh, DID("op_mul")));
		break;
	case BinaryOperator::Specifier::Division:
		ret(m_ir_builder.CreateUDiv(lh, rh, DID("op_div")));
		break;
	default:
		Debug("Compilerbug");
		abort();
		break;
	}

}

void CodeGen::visit(PrefixOpExpr& pre_op)
{
	llvm::Value* expr = get(pre_op.lh);
	auto op_type = UnaryOperator::from_token_specifier(pre_op.op.type);
	switch (op_type)
	{
	case UnaryOperator::Specifier::Plus:
		ret(m_ir_builder.CreateFNeg())
		break;
	case UnaryOperator::Specifier::Minus:
		break;
	default:
		Debug("Compilerbug");
		abort();
	}
}

void CodeGen::visit(PostfixOpExpr& post_op)
{
}

void CodeGen::visit(InferredDeclStmt& decl_inferred)
{
}

void CodeGen::visit(IdentPattern& ident)
{
}

void CodeGen::visit(IdentExpr& ident)
{
}

void CodeGen::visit(NamespaceStmt& namespace_stmt)
{
}

void CodeGen::visit(FuncCallExpr& func_call_expr)
{
}

void CodeGen::visit(FuncDefStmt& func_call_def_stmt)
{
}

void CodeGen::visit(ReturnStmt& ret_stmt)
{
}

void CodeGen::visit(ExprStmt& expr_stmt)
{
}
#endif