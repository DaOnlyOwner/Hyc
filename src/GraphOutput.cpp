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

void GraphOutput::visit(InferredDeclStmt& decl_inferred)
{
	int my_node_name = new_name();
	int fst_child = node + 1;
	out += label(my_node_name, "Declaration Statement (inferred)", ":=");
	decl_inferred.bind_to->accept(*this);
	out += connect(my_node_name, fst_child);
	int snd_child = node + 1;
	decl_inferred.expr->accept(*this);
	out += connect(my_node_name, snd_child);
}

void GraphOutput::visit(IdentPattern& ident)
{
	int node_name = new_name();
	out += label(node_name, "Identifier Pattern", ident.ident.text);
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

int GraphOutput::new_name()
{
	node++;
	return node;
}
