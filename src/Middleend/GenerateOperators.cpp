#include "GenerateOperators.h"

void GenerateOps::visit(TypeDefStmt& td)
{
	Type type_to_generate_for = Type(&td);
	auto decl = std::make_unique<FuncDeclStmt>(scopes.get_type("void")/*The return type of every generated function is void*/,
		nullptr, gen_arg_list(type_to_generate_for));
	auto body = std::vector<uptr<Stmt>>();
	body.reserve(td.stmts.size());
	auto fds = std::make_unique<FuncDefStmt>(std::move(decl), std::move(body));
	
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
		// copy and move op.
		if (decl->arg_list.size() == 2)
		{
			auto& arg1 = decl->arg_list[0];
			auto& arg2 = decl->arg_list[1];
			auto ie = std::make_unique<IdentExpr>(arg1->name);
			auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie));
			auto mcs = std::vector<MatchCase>();
			auto ms = std::make_unique<MatchStmt>(mcs, std::move(pe));
			for (auto& p1 : td.stmts)
			{
				auto ie_inner = std::make_unique<IdentExpr>(arg2->name);
				auto pe_inner = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie_inner));
				auto mcs_inner = std::vector<MatchCase>();
				auto ms_inner = std::make_unique<MatchStmt>(mcs_inner, std::move(pe_inner));
				
				// Do the unions explicitly without traversing the AST. 
				// TODO: This is basically just a hacked solution
				auto lhs = dynamic_cast<UnionDeclStmt*>(p1.get());
				assert(lhs);
				for (auto& p2 : td.stmts)
				{
					auto rhs = dynamic_cast<UnionDeclStmt*>(p2.get());
					assert(rhs);
					auto stmt_opt = gen_func_union({ &lhs->decl_stmt->type,&rhs->decl_stmt->type },scopes,ns);
					if (!stmt_opt.has_value())
					{
						generated_op = false;
						return;
					}
					auto& stmt = stmt_opt.value();
					std::vector<uptr<Stmt>> body;
					if (!stmt)
					{
						body.push_back(std::move(stmt));
					}
					auto match_case = MatchCase(rhs->decl_stmt->name, Token(Token::Specifier::Ident,"rhs"), std::move(body));
					ms_inner->match_cases.push_back(std::move(match_case));
				}

				std::vector<uptr<Stmt>> body;
				body.push_back(std::move(ms_inner));
				auto match_case = MatchCase(lhs->decl_stmt->name, Token(Token::Specifier::Ident, "lhs"), std::move(body));
				ms->match_cases.push_back(std::move(match_case));
			}
			fds->body.push_back(std::move(ms));
		}

		// Destructor
		else if (decl->arg_list.size() == 1)
		{
			auto& arg1 = decl->arg_list[0];
			auto& arg2 = decl->arg_list[1];
			auto ie = std::make_unique<IdentExpr>(arg1->name);
			auto pe = std::make_unique<PrefixOpExpr>(Token(Token::Specifier::Asterix, "*"), std::move(ie));
			auto mcs = std::vector<MatchCase>();
			auto ms = std::make_unique<MatchStmt>(mcs, std::move(pe));
			for (auto& p1 : td.stmts)
			{
				auto lhs = dynamic_cast<UnionDeclStmt*>(p1.get());
				auto stmt_opt = gen_func_union({ lhs->decl_stmt.get()}, scopes, ns);
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
	assert(scopes.add(fds.get()));
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
	RETURN(gen_func_struct({ &decl },scopes,ns));
}
