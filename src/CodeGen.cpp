#include "CodeGen.h"
#include "SymbolTable.h"

CodeGen::CodeGen()
{
}

void CodeGen::visit(FloatLiteralExpr& lit)
{
}

void CodeGen::visit(IntegerLiteralExpr& lit)
{
	auto spec = Primitive::from_token_specifier(lit.integer_literal.type);
	switch (spec)
	{
    case Primitive::Specifier::u8:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(8, lit.integer_literal.val, false);
        break;
    case Primitive::Specifier::u16:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(16, lit.integer_literal.val, false);
        break;
    case Primitive::Specifier::u32:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(32, lit.integer_literal.val, false);
        break;
    case Primitive::Specifier::u64:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(64, lit.integer_literal.val, false);
        break;
    case Primitive::Specifier::s8:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(8, lit.integer_literal.val, true);
        break;
    case Primitive::Specifier::s16:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(16, lit.integer_literal.val, true);
        break;
    case Primitive::Specifier::s32:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(32, lit.integer_literal.val, true);
        break;
    case Primitive::Specifier::s64:
        ret(llvm::ConstantInt::get(m_context, llvm::APInt(64, lit.integer_literal.val, true);
        break;
    default:
        abort();
	}
}

void CodeGen::visit(BinOpExpr& bin_op)
{
}

void CodeGen::visit(PrefixOpExpr& pre_op)
{
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
