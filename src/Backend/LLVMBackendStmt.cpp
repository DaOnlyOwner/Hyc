#include "LLVMBackendStmt.h"
#include "Mangling.h"
#include "fmt/core.h"

void LLVMBackendStmt::visit(FuncDefStmt& func_def)
{
	current_function = be.mod.getFunction(mangle(*func_def.decl));
	current_func_def = &func_def;
	expr_getter.set_curr_func_name(current_func_def->decl->name.text);
	for (auto& p : func_def.body) p->accept(*this);
	current_function = nullptr;
	current_func_def = nullptr;

}

void LLVMBackendStmt::visit(IfStmt& if_stmt)
{
	
	auto if_cond = expr_getter.gen(*if_stmt.if_expr);
	llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(be.context, "then", current_function);
	std::vector<std::pair<llvm::BasicBlock*,llvm::BasicBlock*>> elifs_bb;
	for (int i = 0; i < if_stmt.elif_exprs.size(); i++)
	{
		elifs_bb.push_back(
			{ llvm::BasicBlock::Create(be.context, fmt::format("elif_{}_", i), current_function),
			llvm::BasicBlock::Create(be.context,fmt::format("elif_then_{}_",i)) });
	}
	llvm::BasicBlock* else_bb = nullptr;
	if (!if_stmt.else_stmts.empty())
	{
		else_bb = llvm::BasicBlock::Create(be.context, "else", current_function);
	}
	auto cont_bb = llvm::BasicBlock::Create(be.context, "cont", current_function);

	if (!elifs_bb.empty())
	{
		be.builder.CreateCondBr(if_cond, then_bb, elifs_bb[0].first);
		current_function->getBasicBlockList().push_back(then_bb);
		be.builder.SetInsertPoint(then_bb);
		for (auto& p : if_stmt.if_stmts) p->accept(*this);
		for (int i = 0; i < elifs_bb.size()-1; i++)
		{
			current_function->getBasicBlockList().push_back(elifs_bb[i].first);
			be.builder.SetInsertPoint(elifs_bb[i].first);
			auto elif_cond = expr_getter.gen(*if_stmt.elif_exprs[i]);
			be.builder.CreateCondBr(elif_cond, elifs_bb[i].second, elifs_bb[i + 1].first);
			current_function->getBasicBlockList().push_back(elifs_bb[i].second);
			be.builder.SetInsertPoint(elifs_bb[i].second);
			for (auto& p : if_stmt.all_elif_stmts[i]) p->accept(*this);
		}

		auto& last = elifs_bb.back();
		auto& last_expr = if_stmt.elif_exprs.back();
		auto& last_stmts = if_stmt.all_elif_stmts.back();
		current_function->getBasicBlockList().push_back(last.first);
		be.builder.SetInsertPoint(last.first);
		auto elif_cond = expr_getter.gen(*last_expr);
		be.builder.CreateCondBr(elif_cond, last.second, else_bb ? else_bb : cont_bb);
		current_function->getBasicBlockList().push_back(last.second);
		be.builder.SetInsertPoint(last.second);
		for (auto& p : last_stmts) p->accept(*this);
	}
	else
	{
		be.builder.CreateCondBr(if_cond, then_bb, else_bb ? else_bb : cont_bb);
		current_function->getBasicBlockList().push_back(then_bb);
		be.builder.SetInsertPoint(then_bb);
		for (auto& p : if_stmt.if_stmts) p->accept(*this);
	}

	if (else_bb)
	{
		current_function->getBasicBlockList().push_back(else_bb);
		be.builder.SetInsertPoint(else_bb);
		for (auto& p : if_stmt.if_stmts) p->accept(*this);
		be.builder.CreateBr(cont_bb);
	}
}

void LLVMBackendStmt::visit(WhileStmt& while_stmt)
{
	
}
