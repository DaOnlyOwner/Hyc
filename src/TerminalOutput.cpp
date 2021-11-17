#include "TerminalOutput.h"
#include "fmt/core.h"


void TerminalOutput::make_indent()
{
	for (int i = 0; i < indent; i++)
	{
		fmt::print("  ");
	}
}

void TerminalOutput::visit(FloatLiteralExpr& lit)
{
}

void TerminalOutput::visit(IntegerLiteralExpr& lit)
{
	make_indent();
	fmt::print("IntegerLiteralExpr: {}\n", lit.integer_literal.text);
}

void TerminalOutput::visit(BinOpExpr& bin_op)
{
	make_indent();
	fmt::print("BinOpExpr: {}", bin_op.op.text);
	indent++;
	bin_op.lh->accept(*this);
	bin_op.rh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PrefixOpExpr& pre_op)
{
	make_indent();
	fmt::print("BinOpExpr: {}", pre_op.op.text);
	indent++;
	pre_op.lh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PostfixOpExpr& post_op)
{
	make_indent();
	fmt::print("BinOpExpr: {}", post_op.op.text);
	indent++;
	post_op.rh->accept(*this);
	indent--;
}

void TerminalOutput::visit(DeclCpyStmt& decl_cpy)
{
	make_indent();
	fmt::print("DeclCpyStmt: name={}, type={}", decl_cpy.name.text,decl_cpy.type->as_str());
	indent++;
	decl_cpy.expr->accept(*this);
}

void TerminalOutput::visit(DeclMvStmt& decl_mv)
{
}

void TerminalOutput::visit(DeclInitStmt& decl_init)
{
}

void TerminalOutput::visit(IdentExpr& ident)
{
}

void TerminalOutput::visit(NamespaceStmt& namespace_stmt)
{
}

void TerminalOutput::visit(FuncCallExpr& func_call_expr)
{
}

void TerminalOutput::visit(FuncDefStmt& func_call_def_stmt)
{
}

void TerminalOutput::visit(ReturnStmt& ret_stmt)
{
}

void TerminalOutput::visit(ExprStmt& expr_stmt)
{
}

void TerminalOutput::visit(DeclStmt& def_stmt)
{
}

void TerminalOutput::visit(PointerTypeSpec& pt_spec)
{
}

void TerminalOutput::visit(BaseTypeSpec& bt_spec)
{
}

void TerminalOutput::visit(ArrayTypeSpec& at_spec)
{
}

void TerminalOutput::visit(ImplicitCastExpr& ice)
{
}

void TerminalOutput::visit(IfStmt& if_stmt)
{
}

void TerminalOutput::visit(WhileStmt& while_stmt)
{
}

void TerminalOutput::visit(ContractDefStmt& contract_def_stmt)
{
}

void TerminalOutput::visit(ContractImplStmt& contract_impl_stmt)
{
}

void TerminalOutput::visit(ForStmt& for_stmt)
{
}
