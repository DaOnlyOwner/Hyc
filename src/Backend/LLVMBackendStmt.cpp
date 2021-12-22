#include "LLVMBackendStmt.h"
#include "Mangling.h"
#include "fmt/color.h"
#include "TypeToLLVMType.h"
#include "llvm/IR/DerivedTypes.h"

void LLVMBackendStmt::visit(IfStmt& if_stmt)
{
	auto current_function = be.builder.GetInsertBlock()->getParent();
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
		insert_bb(then_bb);
		scopes.descend();
		for (auto& p : if_stmt.if_stmts) p->accept(*this);
		for (int i = 0; i < elifs_bb.size()-1; i++)
		{
			insert_bb(elifs_bb[i].first);
			auto elif_cond = expr_getter.gen(*if_stmt.elif_exprs[i]);
			if(!elifs_bb[i].first->back().isTerminator())
				be.builder.CreateCondBr(elif_cond, elifs_bb[i].second, elifs_bb[i + 1].first);
			insert_bb(elifs_bb[i].second);
			for (auto& p : if_stmt.all_elif_stmts[i]) p->accept(*this);
		}

		auto& last = elifs_bb.back();
		auto& last_expr = if_stmt.elif_exprs.back();
		auto& last_stmts = if_stmt.all_elif_stmts.back();
		insert_bb(last.first);
		auto elif_cond = expr_getter.gen(*last_expr);
		if (!last.first->back().isTerminator())
			be.builder.CreateCondBr(elif_cond, last.second, else_bb ? else_bb : cont_bb);
		insert_bb(last.second);
		scopes.descend();
		for (auto& p : last_stmts) p->accept(*this);
	}
	else
	{
		be.builder.CreateCondBr(if_cond, then_bb, else_bb ? else_bb : cont_bb);
		insert_bb(then_bb);
		scopes.descend();
		for (auto& p : if_stmt.if_stmts) p->accept(*this);
	}

	if (else_bb)
	{
		insert_bb(else_bb);
		if (!if_stmt.else_stmts.empty()) scopes.descend();
		for (auto& p : if_stmt.else_stmts) p->accept(*this);
		if(else_bb->back().isTerminator())
			be.builder.CreateBr(cont_bb);
	}
	insert_bb(cont_bb);
}

void LLVMBackendStmt::visit(WhileStmt& while_stmt)
{
	auto current_function = be.builder.GetInsertBlock()->getParent();
	auto while_start_bb = llvm::BasicBlock::Create(be.context, "while_start", current_function);
	auto body_bb = llvm::BasicBlock::Create(be.context, "body", current_function);
	auto while_end_bb = llvm::BasicBlock::Create(be.context, "while_end", current_function);
	insert_bb(while_start_bb);
	auto cond = expr_getter.gen(*while_stmt.expr);
	be.builder.CreateCondBr(cond, body_bb, while_end_bb);
	insert_bb(body_bb);
	scopes.descend();
	for (auto& p : while_stmt.stmts) p->accept(*this);
	be.builder.CreateBr(while_start_bb);
	insert_bb(while_end_bb);
}

void LLVMBackendStmt::visit(DeclStmt& decl_stmt)
{
	auto alloc = create_alloca(decl_stmt.type, decl_stmt.name.text);
	scopes.add(decl_stmt.name.text,alloc);
	RETURN(alloc);
}

void LLVMBackendStmt::visit(FuncDeclStmt& func_decl)
{
	auto currfn = get_curr_fn();
	int idx = 0;
	for (auto& arg : currfn->args())
	{
		auto& p = func_decl.arg_list[idx];
		auto alloc = get(p);
		be.builder.CreateStore(&arg, alloc);
		idx++;
	}
}

void LLVMBackendStmt::visit(ExprStmt& expr_stmt)
{
	expr_getter.gen(*expr_stmt.expr);
}

void LLVMBackendStmt::visit(NamespaceStmt& ns)
{
	scopes.descend();
	for (auto& p : ns.stmts) p->accept(*this);
}

void LLVMBackendStmt::visit(MatchStmt& ms)
{
	auto curr_fn = get_curr_fn();
	std::vector<llvm::BasicBlock*> bbs;
	for (auto& c : ms.match_cases)
	{
		bbs.push_back(llvm::BasicBlock::Create(be.context, fmt::format("lb_{}", c.var.text), curr_fn));
	}
	auto switch_end = llvm::BasicBlock::Create(be.context, "switch_end",curr_fn);

	auto expr = expr_getter.gen(*ms.match_on, false);
	auto addr = be.builder.CreateStructGEP(expr, UNION_TAG);
	auto val = be.builder.CreateLoad(llvm::Type::getInt64Ty(be.context), addr);
	auto inst = be.builder.CreateSwitch(val, switch_end,ms.match_cases.size());
	for (size_t i = 0; i < ms.match_cases.size(); i++)
	{
		auto& case_ = ms.match_cases[i];
		auto udecl = scopes.get_union_decl_for(ms.match_on->sem_type.get_base_type(), case_.var.text);
		auto ci = create_ci(udecl->tagged_value->val, be.context);
		inst->addCase(ci, bbs[i]);
	}

	for (int i = 0; i < ms.match_cases.size(); i++)
	{
		scopes.descend();
		auto& case_ = ms.match_cases[i];
		auto& bb = bbs[i];
		be.builder.SetInsertPoint(bb);
		if (case_.as.has_value())
		{
			auto alloc = create_alloca(case_.as_decl->type, case_.as.value().text);
			scopes.add(case_.as.value().text, alloc);

			auto udecl = scopes.get_union_decl_for(ms.match_on->sem_type.get_base_type(), case_.var.text);
			auto addr = create_casted_union_field(scopes, udecl, be, expr);

			be.builder.CreateStore(addr, alloc);
		}
		for (auto& p : case_.body) p->accept(*this);
		if(!be.builder.GetInsertBlock()->back().isTerminator())
			be.builder.CreateBr(switch_end);

	}
	be.builder.SetInsertPoint(switch_end);

}

void LLVMBackendStmt::visit(FuncDefStmt& func_def)
{
	if (!func_def.decl->generic_list.empty()) return;
	scopes.descend();
	auto func = be.mod.getFunction(mangle(*func_def.decl));
	auto entry_bb = llvm::BasicBlock::Create(be.context, "entry", func);
	be.builder.SetInsertPoint(entry_bb);
	//enter_function(func);
	func_def.decl->accept(*this);
	for (auto& p : func_def.body) p->accept(*this);
	auto curr_fn = get_curr_fn();
	if (!curr_fn->back().back().isTerminator())
	{
		be.builder.CreateUnreachable();
	}
}

void LLVMBackendStmt::visit(ReturnStmt& return_stmt)
{
	// TODO: Support return of void
	if (!return_stmt.returned_expr)
		be.builder.CreateRetVoid();
	else 
		be.builder.CreateRet(expr_getter.gen(*return_stmt.returned_expr,true));
}


//inline void LLVMBackendStmt::enter_function(llvm::Function* func)
//{
//	for (auto& arg : func->args())
//	{
//		scopes.add(arg.getName().str(), &arg);
//	}
//}

llvm::AllocaInst* LLVMBackendStmt::create_alloca(const Type& t, const std::string& name)
{
	auto mapped = map_type(t,scopes,be.context);
	auto f = be.builder.GetInsertBlock()->getParent();
	auto entry_b = &f->getEntryBlock();
	llvm::IRBuilder<> tmp(entry_b, entry_b->begin());
	return tmp.CreateAlloca(mapped,0,name);
}

