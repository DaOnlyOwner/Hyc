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
		for (auto& p : td.stmts)
		{
			auto& gen_stmt_opt = get(*p);
			if (!gen_stmt_opt.has_value())
			{
				generated_op = false;
				return;
			}
			auto& gen_stmt = gen_stmt_opt.value();
			if (gen_stmt)
			{
				fds->body.push_back(std::move(gen_stmt));
			}

		}
	}

	// Union
	else
	{
		// Automatically generating move and copy constructors for unions is problematic
		// Consider this:
		/*
		union A
		{
		   B b;
		   float f;
		}

		A foo;
		A bar;
		foo.b = [...]
		bar.f = 0;
		bar = foo;
		This calls somewhere =(float* a, B* b);
		What is the semantic of this call?
		*/
		// 
		//// copy and move op.
		//if (fds->decl->arg_list.size() == 2)
		//{
		//	auto& arg1 = fds->decl->arg_list[0];
		//	auto& arg2 = fds->decl->arg_list[1];
		//	auto ie = std::make_unique<IdentExpr>(arg1->name);
		//	auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie));
		//	auto mcs = std::vector<MatchCase>();
		//	auto ms = std::make_unique<MatchStmt>(std::move(mcs), std::move(pe));
		//	for (auto& p1 : td.stmts)
		//	{
		//		auto ie_inner = std::make_unique<IdentExpr>(arg2->name);
		//		auto pe_inner = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie_inner));
		//		auto mcs_inner = std::vector<MatchCase>();
		//		auto ms_inner = std::make_unique<MatchStmt>(std::move(mcs_inner), std::move(pe_inner));
		//		
		//		// Do the unions explicitly without traversing the AST. 
		//		// TODO: This is basically just a hacked solution
		//		auto lhs = dynamic_cast<UnionDeclStmt*>(p1.get());
		//		assert(lhs);
		//		for (auto& p2 : td.stmts)
		//		{
		//			auto rhs = dynamic_cast<UnionDeclStmt*>(p2.get());
		//			assert(rhs);
		//			auto stmt_opt = gen_func_union(&td,{ lhs->decl_stmt.get(),rhs->decl_stmt.get() }, scopes, ns);
		//			if (!stmt_opt.has_value())
		//			{
		//				generated_op = false;
		//				return;
		//			}
		//			auto& stmt = stmt_opt.value();
		//			std::vector<uptr<Stmt>> body;
		//			if (!stmt)
		//			{
		//				body.push_back(std::move(stmt));
		//			}
		//			auto match_case = MatchCase(rhs->decl_stmt->name, Token(Token::Specifier::Ident,"rhs"), std::move(body));
		//			ms_inner->match_cases.push_back(std::move(match_case));
		//		}

		//		std::vector<uptr<Stmt>> body;
		//		body.push_back(std::move(ms_inner));
		//		auto match_case = MatchCase(lhs->decl_stmt->name, Token(Token::Specifier::Ident, "lhs"), std::move(body));
		//		ms->match_cases.push_back(std::move(match_case));
		//	}
		//	fds->body.push_back(std::move(ms));
		//}

		// Destructor
		if (fds->decl->arg_list.size() == 1)
		{
			auto& arg1 = fds->decl->arg_list[0];
			auto& arg2 = fds->decl->arg_list[1];
			auto ie = std::make_unique<IdentExpr>(arg1->name);
			auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie));
			auto mcs = std::vector<MatchCase>();
			auto ms = std::make_unique<MatchStmt>(std::move(mcs), std::move(pe));
			for (auto& p1 : td.stmts)
			{
				auto lhs = dynamic_cast<UnionDeclStmt*>(p1.get());
				auto stmt_opt = gen_func_union(&td,lhs->decl_stmt.get(), scopes, ns);
				if (!stmt_opt.has_value())
				{
					generated_op = false;
					return;
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
		}
	}
	assert(scopes.add_op(fds.get()));
	generated_op = true;
	ns.stmts.push_back(std::move(fds));
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
