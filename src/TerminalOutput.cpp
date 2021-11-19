#include "TerminalOutput.h"
#include "fmt/printf.h"


void TerminalOutput::make_indent()
{
	for (int i = 0; i < indent; i++)
	{
		out += "  ";
	}
}

void TerminalOutput::visit(ContinueStmt& cont_stmt)
{
	make_indent();
	out += "ContinueStmt\n";
}

void TerminalOutput::visit(FptrTypeSpec& fptr)
{
}

void TerminalOutput::visit(StructDefStmt& struct_def_stmt)
{
	make_indent();
	std::string gp = "";
	for (auto& param : struct_def_stmt.generic_params)
	{
		gp += fmt::format("{}={}, ", param.name.text, param.default_type != nullptr ? param.default_type->as_str() : "");
	}
	out += "StructDefStmt: name='{}', generic_params='{}'\n",struct_def_stmt.name.text,gp;
	indent++;
	for (auto& stmt : struct_def_stmt.stmts)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(FloatLiteralExpr& lit)
{
	assert(false);
}

void TerminalOutput::visit(IntegerLiteralExpr& lit)
{
	make_indent();
	out += fmt::format("IntegerLiteralExpr: '{}'\n", lit.integer_literal.text);
}

void TerminalOutput::visit(BinOpExpr& bin_op)
{
	make_indent();
	out += fmt::format("BinOpExpr: '{}'\n", bin_op.op.text);
	indent++;
	bin_op.lh->accept(*this);
	bin_op.rh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PrefixOpExpr& pre_op)
{
	make_indent();
	out+=fmt::format("BinOpExpr: '{}'\n", pre_op.op.text);
	indent++;
	pre_op.lh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PostfixOpExpr& post_op)
{
	make_indent();
	out += fmt::format("BinOpExpr: '{}'\n", post_op.op.text);
	indent++;
	post_op.rh->accept(*this);
	indent--;
}

void TerminalOutput::visit(DeclCpyStmt& decl_cpy)
{
	make_indent();
	out += fmt::format("DeclCpyStmt: name='{}', type='{}'\n", decl_cpy.name.text,decl_cpy.type == nullptr ? "not specified" : decl_cpy.type->as_str());
	indent++;
	decl_cpy.expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(DeclMvStmt& decl_mv)
{
	make_indent();
	out += fmt::format("DeclMvStmt: name='{}', type='{}'\n", decl_mv.name.text, decl_mv.type == nullptr ? "not specified" : decl_mv.type->as_str());
	indent++;
	decl_mv.expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(DeclInitStmt& decl_init)
{
	make_indent();
	out += fmt::format("DeclInitStmt: name='{}', type='{}'\n", decl_init.name.text, decl_init.type == nullptr ? "not specified" : decl_init.type->as_str());
	indent++;
	decl_init.expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(IdentExpr& ident)
{
	make_indent();
	out += fmt::format("IdentExpr: name='{}'\n", ident.ident.text);
}

void TerminalOutput::visit(NamespaceStmt& namespace_stmt)
{
	make_indent();
	out += fmt::format("NamespaceStmt: name='{}'\n", namespace_stmt.name.text);
	indent++;
	for (auto& stmt : namespace_stmt.stmts)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(FuncCallExpr& func_call_expr)
{
	make_indent();
	out+=fmt::format("FuncCallExpr: name='{}'\n", func_call_expr.name.text);
	indent++;
	for (int i = 0; i < func_call_expr.arg_list.size(); i++)
	{
		func_call_expr.arg_list[i]->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(FuncDefStmt& func_def_stmt)
{
	make_indent();
	std::string args = "";
	for (int i = 0; i < func_def_stmt.arg_list_type_ident.size(); i++)
	{
		auto& arg = func_def_stmt.arg_list_type_ident[i];
		args += fmt::format("{} {}, ", arg.first->as_str(), arg.second.text);
	}
	std::string generic_params = "";
	for (auto& gi : func_def_stmt.generic_list)
	{
		std::string needed_ctrcts = "";
		for (auto& contr : gi.needed_contracts)
		{
			needed_ctrcts += fmt::format("{}+", contr.text);
		}
		generic_params += fmt::format("{}:{}={},", gi.name.text, needed_ctrcts, gi.default_type->as_str());
	}
	out += fmt::format("FuncDefStmt: name='{}', generic_params={}, return_type='{}', arg_list='{}'\n",func_def_stmt.name.text, generic_params, func_def_stmt.ret_type->as_str(), args);
	indent++;
	for (auto& stmt : func_def_stmt.body)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(ReturnStmt& ret_stmt)
{
	make_indent();
	out += "ReturnStmt\n";
	indent++;
	ret_stmt.returned_expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(ExprStmt& expr_stmt)
{
	make_indent();
	out += "ExprStmt\n";
	indent++;
	expr_stmt.expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(DeclStmt& decl_stmt)
{
	make_indent();
	out += fmt::format("DeclStmt: name='{}', type='{}'\n", decl_stmt.name.text, decl_stmt.type_spec->as_str());
}

void TerminalOutput::visit(PointerTypeSpec& pt_spec)
{
	assert(false);
}

void TerminalOutput::visit(BaseTypeSpec& bt_spec)
{
	assert(false);
}

void TerminalOutput::visit(ArrayTypeSpec& at_spec)
{
	assert(false);
}

void TerminalOutput::visit(ImplicitCastExpr& ice)
{
	make_indent();
	out += fmt::format("DeclStmt: cast_to_type='{}'\n", ice.sem_type.as_str());
	indent++;
	ice.expr->accept(*this);
	indent--;
}

void TerminalOutput::visit(IfStmt& if_stmt)
{
	make_indent();
	out += "IfStmt\n";
	make_indent();
	indent++;
	if_stmt.if_expr->accept(*this);
	for (auto& body_if : if_stmt.if_stmts)
	{
		body_if->accept(*this);
	}
	indent--;
	for (int i = 0; i < if_stmt.elif_exprs.size(); i++)
	{
		make_indent();
		out += "ElifStmt\n";
		auto& elif_expr = if_stmt.elif_exprs[i];
		auto& elif_stmts = if_stmt.all_elif_stmts[i];
		indent++;
		elif_expr->accept(*this);
		indent--;
		for (int j = 0; j < elif_stmts.size(); j++)
		{
			indent++;
			auto& elif_stmt = elif_stmts[j];
			elif_stmt->accept(*this);
			indent--;
		}
	}

	make_indent();
	out += "ElseStmt\n";
	indent++;
	for (auto& else_stmt : if_stmt.else_stmts)
	{
		else_stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(WhileStmt& while_stmt)
{
	make_indent();
	out+= "WhileStmt\n";
	indent++;
	while_stmt.expr->accept(*this);
	for (auto& stmt : while_stmt.stmts)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(ContractDefStmt& contract_def_stmt)
{
	make_indent();
	std::string inh = "";
	for (int i = 0; i < contract_def_stmt.inherited_contracts.size(); i++)
	{
		inh += contract_def_stmt.inherited_contracts[i].text;
	}
	out += fmt::format("ContractDefStmt: name='{}', inherited_contracts='{}'\n", contract_def_stmt.name.text,inh);
	indent++;
	for (auto& stmt : contract_def_stmt.stmts)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(ContractImplStmt& contract_impl_stmt)
{
	make_indent();
	out += fmt::format("ContractImplStmt: for='{}', contract_name='{}'\n", contract_impl_stmt.for_name.text, contract_impl_stmt.contr_name.text);
	indent++;
	for (auto& stmt : contract_impl_stmt.func_defs)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(ForStmt& for_stmt)
{
	make_indent();
	out += "ForStmt\n";
	indent++;
	for_stmt.decl_stmt->accept(*this);
	for_stmt.fst_expr->accept(*this);
	for_stmt.snd_expr->accept(*this);
	for (auto& stmt : for_stmt.stmts) stmt->accept(*this);
	indent--;
}
