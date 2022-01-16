#include "GenerateOperators.h"
#include "Mangling.h"

void GenerateOps::visit(TypeDefStmt& td)
{
	Type type_to_generate_for = Type(&td);
	type_to_generate_for.promote_pointer();
	auto ts = std::make_unique<BaseTypeSpec>(Token(Token::Specifier::Ident, "void"));
	ts->semantic_type = scopes.get_type("void");
	auto decl = std::make_unique<FuncDeclStmt>(std::move(ts),
		Token(Token::Specifier::Ident,op_name),nullptr, gen_arg_list(type_to_generate_for));
	decl->is_gen = true;
	auto mangled = mangle(*decl);
	decl->name.text = mangled;
	auto body = std::vector<uptr<Stmt>>();
	body.reserve(td.stmts.size());
	auto fds = std::make_unique<FuncDefStmt>(std::move(decl), std::move(body),true);
	current_def = &td;
	if (scopes.get_op(fds->decl->name.text))
	{
		// Already defined, we don't have to generate it again.
		generated_op = true;
		return;
	}

	if (td.get_ct() == CollectionType::Struct)
	{
		// Destructor
		if (fds->decl->arg_list.size() == 1)
		{
			if (!gen_struct_destr(td, fds.get()))
				return;
		}

		// Move, Copy
		else
		{
			// TODO: single value is not copied.
			if(!gen_struct_movecopy(td, fds))
				return;
		}
	}

	// Union
	else
	{
		// Destructor
		if (fds->decl->arg_list.size() == 1)
		{
			if (!gen_union_destr(fds, td))
				return;
		}
	}
	assert(scopes.add_op(fds.get()));
	generated_op = true;
	ns.stmts.push_back(std::move(fds));
}

bool GenerateOps::gen_union_destr(std::unique_ptr<FuncDefStmt, std::default_delete<FuncDefStmt>>& fds, TypeDefStmt& td)
{
	auto& arg1 = fds->decl->arg_list[0];
	auto& arg2 = fds->decl->arg_list[1];
	auto ie = std::make_unique<IdentExpr>(arg1.decl->name);
	auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie));
	auto mcs = std::vector<MatchCase>();
	auto ms = std::make_unique<MatchStmt>(std::move(mcs), std::move(pe));
	for (auto& p1 : td.stmts)
	{
		auto lhs = dynamic_cast<UnionDeclStmt*>(p1.get());
		auto stmt_opt = gen_func_union(&td, lhs->decl_stmt.get(), scopes, ns);
		if (!stmt_opt.has_value())
		{
			generated_op = false;
			return false;
		}
		auto& stmt = stmt_opt.value();
		std::vector<uptr<Stmt>> body;
		if (stmt)
		{
			body.push_back(std::move(stmt));
		}
		auto match_case = MatchCase(lhs->decl_stmt->name, Token(Token::Specifier::Ident, "matched"), std::move(body));
		ms->match_cases.push_back(std::move(match_case));
	}
	fds->body.push_back(std::move(ms));
	return true;
}


bool GenerateOps::gen_struct_movecopy(TypeDefStmt& td, std::unique_ptr<FuncDefStmt, std::default_delete<FuncDefStmt>>& fds)
{
	int64_t start = 0;
	for (int64_t i = 0; i < td.stmts.size(); i++)
	{
		auto& stmt = td.stmts[i];
		auto* decl = static_cast<DeclStmt*>(stmt.get());
		if (decl->type.is_user_defined(scopes))
		{
			std::unique_ptr<Stmt> gen_stmt;
			int64_t end = i - 1;
			handle_memcpy_op_gen(end, start, td.stmts.size(), td,  gen_stmt);
			auto& gen_stmt2_ = get(*decl);
			if (!gen_stmt2_.has_value())
			{
				generated_op = false;
				return false;
			}
			auto gen_stmt2 = std::move(gen_stmt2_.value());
			assert(gen_stmt2);
			if (gen_stmt)
			{
				fds->body.push_back(std::move(gen_stmt));
			}
			fds->body.push_back(std::move(gen_stmt2));
			start = i + 1;
		}
	}
	if (!td.stmts.empty())
	{
		std::unique_ptr<Stmt> gen_stmt=nullptr;
		handle_memcpy_op_gen(td.stmts.size() - 1, start, td.stmts.size(), td, gen_stmt);
		if (gen_stmt != nullptr)
		{
			fds->body.push_back(std::move(gen_stmt));
		}
	}
}

bool GenerateOps::gen_struct_destr(TypeDefStmt& td, FuncDefStmt* fds)
{
	for (auto& p : td.stmts)
	{
		auto& gen_stmt_opt = get(*p);
		if (!gen_stmt_opt.has_value())
		{
			generated_op = false;
			return false;
		}
		auto& gen_stmt = gen_stmt_opt.value();
		if (gen_stmt)
		{
			fds->body.push_back(std::move(gen_stmt));
		}
	}
	return true;
}


void GenerateOps::handle_memcpy_op_gen(int64_t end, int64_t start, size_t stmt_size, TypeDefStmt& td, std::unique_ptr<Stmt>& gen_stmt)
{
	// Spans over the whole range, but don't memcpy a single 
	if (stmt_size - 1 == end - start && stmt_size > 1)
	{
		auto lhs_a = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "a"));
		auto lhs_b = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "b"));
		auto pre = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), lhs_a->clone());
		auto size = std::make_unique<SizeOrAlignmentInfoExpr>(Token(Token::Specifier::KwSizeof, "sizeof"), std::move(pre));
		auto mem = std::make_unique<MemOpExpr>(std::move(lhs_a), std::move(lhs_b), std::move(size), Token(Token::Specifier::MemCpy, "-->"));
		auto stmt = std::make_unique<ExprStmt>(std::move(mem));
		gen_stmt = std::move(stmt);
	}
	else if (end > 0 && (end - start >= 0))
	{
		auto* mem1 = static_cast<DeclStmt*>(td.stmts[start].get());
		auto* mem2 = static_cast<DeclStmt*>(td.stmts[end].get());
		int64_t dst = end - start;
		// We memcpy if memcpy spans more >= two members
		if (dst >= 1)
		{
			auto size = std::make_unique<SizeBetweenMemberInfoExpr>(&td, mem1->name, mem2->name);

			auto lhs_a = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "a"));
			auto rhs_a = std::make_unique<IdentExpr>(mem1->name);
			auto bin_a = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lhs_a), std::move(rhs_a));
			auto pre_a = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Ampersand, "&"), std::move(bin_a));

			auto lhs_b = std::make_unique<IdentExpr>(Token(Token::Specifier::Ident, "b"));
			auto rhs_b = std::make_unique<IdentExpr>(mem1->name);
			auto bin_b = std::make_unique<BinOpExpr>(Token(Token::Specifier::MemAccess, "->"), std::move(lhs_b), std::move(rhs_b));
			auto pre_b = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Ampersand, "&"), std::move(bin_b));

			auto memcpy_op = std::make_unique<MemOpExpr>(std::move(pre_a), std::move(pre_b),
				std::move(size), Token(Token::Specifier::MemCpy, "-->"));
			gen_stmt = std::make_unique<ExprStmt>(std::move(memcpy_op));
		}
		// Dont copy if it spans less than two members 
		else
		{
			assert(dst >= 0);
			auto& gen_opt = get(*mem2);
			assert(gen_opt.has_value());
			assert(gen_opt.value());
			gen_stmt = std::move(gen_opt.value());
		}
	}
}

void GenerateOps::visit(NamespaceStmt& ns)
{
	int s = ns.stmts.size();
	for (int i = 0; i < s; i++)
	{
		ns.stmts[i]->accept(*this);
	}
}

void GenerateOps::visit(DeclStmt& decl)
{
	RETURN(gen_func_struct(current_def,&decl,scopes,ns));
}

void gen_op(Scopes& sc,
	NamespaceStmt& ns,
	std::function<gen_func_t>& gen_func_struct,
	std::function<gen_func_t>& gen_func_union,
	std::function<gen_arglist_t>& gen_arg_list,
	const std::string& name)
{
	GenerateOps gops(sc, ns, gen_func_struct, gen_func_union, gen_arg_list,name);
	ns.accept(gops);
}
