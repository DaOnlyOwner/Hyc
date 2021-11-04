#include "GraphOutput.h"
#include <fstream>
#include <assert.h>

std::string connect(int nodeFrom, int nodeTo)
{
	return std::to_string(nodeFrom) + "->" + std::to_string(nodeTo) + "\n";
}

std::string label(int node, const std::string& descr, const std::string& text)
{
	return std::to_string(node) + "[label=\"" + descr + "{" + text + "}\"]\n";
}


void GraphOutput::write_to_file(const std::string& filename)
{
	std::ofstream fout;
	fout.open(filename);
	assert(fout.good());
	fout << out + "}";
	fout.close();
}

void GraphOutput::visit(FloatLiteralExpr& lit)
{
	node++;
	out += label(node, Token::Translate(lit.float_literal.type), lit.float_literal.text);
}

void GraphOutput::visit(IntegerLiteralExpr& lit)
{
	node++;
	out += label(node, Token::Translate(lit.integer_literal.type), lit.integer_literal.text);
}

void GraphOutput::visit(BinOpExpr& bin_op)
{
	node++;
	int my_node_name = node;
	int node_fst_child = node+1;
	out += label(node, "Binary Expression", bin_op.op.text);
	bin_op.lh->accept(*this);
	out += connect(my_node_name, node_fst_child);
	int node_second_child = node + 1;
	bin_op.rh->accept(*this);
	out += connect(my_node_name, node_second_child);
}

void GraphOutput::visit(PrefixOpExpr& pre_op)
{
	node++;
	int my_node_name = node;
	int node_fst_child = node + 1;
	out += label(my_node_name, "Unary Expression (Prefix)", pre_op.op.text);
	pre_op.lh->accept(*this);
	out += connect(my_node_name, node_fst_child);
}

void GraphOutput::visit(PostfixOpExpr& post_op)
{
	assert(false);
	node++;
	int my_node_name = node;
	int node_fst_child = node + 1;
	out += label(node, "Unary Expression (Postfix)", post_op.op.text);
	post_op.rh->accept(*this);
	out += connect(my_node_name, node_fst_child);
}

void GraphOutput::visit(InferredDefStmt& decl)
{
	int my_node_name = new_name();
	out += label(my_node_name, "Definition Statement (inferred)", ":=");
	
	int child_name = new_name();
	out += label(child_name, "Name", decl.name.text);

	out += connect(my_node_name, child_name);
	int snd_child = node + 1;
	decl.expr->accept(*this);
	out += connect(my_node_name, snd_child);
}

void GraphOutput::visit(DefStmt& def_stmt)
{
	int node_name = new_name();
	int child1 = node + 1;
	out += label(node_name, "Definition Statement", "=");

	int cn2 = new_name();
	out += label(cn2, "Type", def_stmt.type_tkn.text);
	out += connect(node_name, cn2);

	int child_name = new_name();
	out += label(child_name, "Name", def_stmt.name.text);
	out += connect(node_name, child_name);

	int expr_child = node + 1;
	def_stmt.expr->accept(*this);
	out += connect(node_name, expr_child);
}


void GraphOutput::visit(IdentExpr& ident)
{
	int node_name = new_name();
	out += label(node_name, "Identifier Expression", ident.ident.text);
}

void GraphOutput::visit(NamespaceStmt& nms_stmt)
{
	int node_name = new_name();
	int child_name = node + 1;
	out += label(node_name, "Namespace Stmt", nms_stmt.name.text);
	for (uptr<Stmt>& stmt : nms_stmt.stmts)
	{
		stmt->accept(*this);
		out += connect(node_name, child_name);
		child_name = node + 1;
	}
}
void GraphOutput::visit(FuncCallExpr& func_call_expr)
{
	int node_name = new_name();
	int child_name = node + 1;
	out += label(node_name, "FuncCallExpr", func_call_expr.name.text);
	for (int i = 0; i < func_call_expr.arg_list.size(); i++)
	{
		auto& arg = func_call_expr.arg_list[i];
		arg->accept(*this);
		out += connect(node_name, child_name);
		child_name = node + 1;
	}
}

void GraphOutput::visit(FuncDefStmt& func_call_def_stmt)
{
	int node_name = new_name();
	out += label(node_name, "FuncDefStmt", "name: " + func_call_def_stmt.name.text + "| type: " + func_call_def_stmt.ret_type.text);
	for (int i = 0; i < func_call_def_stmt.arg_list_type_ident.size(); i++)
	{
		auto& arg = func_call_def_stmt.arg_list_type_ident[i];
		int child_name = new_name();
		out += label(child_name, "TypeIdentPair (parameter)", "name: " + arg.first.text + "| type: " + arg.second.text);
		out += connect(node_name, child_name);
	}

	int child_name = node + 1;
	for (int i = 0; i < func_call_def_stmt.body.size(); i++)
	{
		auto& arg = func_call_def_stmt.body[i];
		arg->accept(*this);
		out += connect(node_name, child_name);
		child_name = node + 1;
	}


}

void GraphOutput::visit(ReturnStmt& ret_stmt)
{
	int node_name = new_name();
	int child_name = node + 1;
	out += label(node_name, "ReturnStmt","");
	ret_stmt.returned_expr->accept(*this);
	out += connect(node_name, child_name);
}

void GraphOutput::visit(ExprStmt& expr_stmt)
{
	int node_name = new_name();
	int child_name = node + 1;
	out += label(node_name, "ExprStmt", "");
	expr_stmt.expr->accept(*this);
	out += connect(node_name, child_name);
}

int GraphOutput::new_name()
{
	node++;
	return node;
}

