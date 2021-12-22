#include "TerminalOutput.h"
#include "fmt/printf.h"
#include <cassert>


void TerminalOutput::make_indent()
{
	for (int i = 0; i < indent; i++)
	{
		out += "  ";
	}
}

void TerminalOutput::visit(MatchStmt& match)
{
	make_indent();
	out += "MatchStmt\n";
	indent++;
	match.match_on->accept(*this);
	for (MatchCase& mc : match.match_cases)
	{
		make_indent();
		out += fmt::format("MatchCase: matched='{}', as='{}'\n", mc.var.text, mc.as.has_value() ? mc.as.value().text : "");
		indent++;
		for (auto& stmt : mc.body)
			stmt->accept(*this);
		indent--;
	}
	indent--;
}

void TerminalOutput::visit(ScopeStmt& sc)
{
	make_indent();
	out += "ScopeStmt\n";
	indent++;
	for (auto& p : sc.stmts)p->accept(*this);
	indent--;
}

void TerminalOutput::visit(FuncDeclStmt& func_decl)
{
	make_indent();
	std::string args = "";
	for (int i = 0; i < func_decl.arg_list.size(); i++)
	{
		auto& arg = func_decl.arg_list[i];
		args += fmt::format("{} {}, ", arg->type_spec->as_str(), arg->name.text);
	}
	std::string generic_params = "";
	for (auto& gi : func_decl.generic_list)
	{
		generic_params += fmt::format("{}{},", gi.name.text, gi.default_type ? "=" + gi.default_type->as_str() : "");
	}
	out += fmt::format("FuncDeclStmt: name='{}', generic_params={}, return_type='{}', arg_list='{}'\n", func_decl.name.text, generic_params, func_decl.ret_type->as_str(), args);
}

void TerminalOutput::visit(TypeDefStmt& coll_def)
{
	make_indent();
	std::string gp = "";
	for (auto& param : coll_def.generic_params)
	{
		gp += fmt::format("{}{}, ", param.name.text, param.default_type != nullptr ? "=" + param.default_type->as_str() : "");
	}
	out += fmt::format("TypeDefStmt: collection_type='{}', name='{}', generic_params='{}'\n", coll_def.get_collection_type(), coll_def.name.text, gp);
	indent++;
	for (auto& stmt : coll_def.stmts)
	{
		stmt->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(UnionDeclStmt& udecl)
{
	make_indent();
	out += fmt::format("UnionDeclStmt: tagged_value='{}'\n",udecl.tagged_value.value().val);
	indent++;
	udecl.decl_stmt->accept(*this);
	indent--;
}

void TerminalOutput::visit(ContinueStmt& cont_stmt)
{
	make_indent();
	out += "ContinueStmt\n";
}

void TerminalOutput::visit(FptrTypeSpec& fptr)
{
	assert(false);
}

void TerminalOutput::visit(ArraySubscriptExpr& subs)
{
	make_indent();
	out += "ArraySubscriptExpr\n";
	indent++;
	subs.inner->accept(*this);
	subs.from->accept(*this);
	indent--;
}

void TerminalOutput::visit(TernaryExpr& tern)
{
	make_indent();
	out += "TernaryExpr\n";
	make_indent();
	out += "First Expr:\n";
	indent++;
	tern.fst->accept(*this);
	indent--;
	make_indent();
	out += "Second Expr:\n";
	indent++;
	tern.snd->accept(*this);
	indent--;
	make_indent();
	out += "Third Expr:\n";
	indent++;
	tern.trd->accept(*this);
	indent--;
}

void TerminalOutput::visit(DecimalLiteralExpr& lit)
{
	make_indent();
	out += fmt::format("DecimalLiteralExpr: '{}', semantic_type='{}'", lit.lit.text, lit.sem_type.as_str());
}

void TerminalOutput::visit(IntegerLiteralExpr& lit)
{
	make_indent();
	out += fmt::format("IntegerLiteralExpr: '{}', semantic_type='{}'\n", lit.integer_literal.text,lit.sem_type.as_str());
}

void TerminalOutput::visit(BinOpExpr& bin_op)
{
	make_indent();
	out += fmt::format("BinOpExpr: '{}', semantic_type='{}'\n", bin_op.op.text, bin_op.sem_type.as_str());
	indent++;
	bin_op.lh->accept(*this);
	bin_op.rh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PrefixOpExpr& pre_op)
{
	make_indent();
	out+=fmt::format("PrefixOpExpr: '{}', semantic_type='{}'\n", pre_op.op.text,pre_op.sem_type.as_str());
	indent++;
	pre_op.lh->accept(*this);
	indent--;
}

void TerminalOutput::visit(PostfixOpExpr& post_op)
{
	make_indent();
	out += fmt::format("PostfixOpExpr: '{}', semantic_type='{}'\n", post_op.op.text,post_op.sem_type.as_str());
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

void TerminalOutput::visit(IdentExpr& name)
{
	make_indent();
	std::string gargs = "";
	for (auto& expr : name.generic_params)
		gargs += fmt::format("{}, ", expr->as_str());
	out += fmt::format("IdentExpr: name='{}', generic_params='{}', semantic_type='{}'\n", name.name.text,gargs,name.sem_type.as_str());
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

void TerminalOutput::visit(FuncCallExpr& func_op)
{
	make_indent();
	out += "FuncOpCallExpr\n";

	indent++;
	func_op.from->accept(*this);
	indent--;

	make_indent();
	out += "Args:\n";
	indent++;
	for (int i = 0; i < func_op.arg_list.size(); i++)
	{
		func_op.arg_list[i].expr->accept(*this);
	}
	indent--;
}

void TerminalOutput::visit(FuncDefStmt& func_def_stmt)
{
	make_indent();
	out += "FuncDefStmt\n";
	indent++;
	func_def_stmt.decl->accept(*this);
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
	out += fmt::format("DeclStmt: name='{}', type='{}', semantic_type='{}'\n", decl_stmt.name.text, decl_stmt.type_spec ? decl_stmt.type_spec->as_str() : "",decl_stmt.type.as_str());
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
	out += fmt::format("ImplicitCastExpr: cast_to_type='{}'\n", ice.sem_type.as_str());
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
