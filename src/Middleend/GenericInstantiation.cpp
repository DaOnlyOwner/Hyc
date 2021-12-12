#include "GenericInstantiation.h"
#include "DebugPrint.h"
#include <algorithm>
#include "Messages.h"

namespace
{
	bool contains(std::vector<GenericInfo>* gi, const std::string& text)
	{
		if (gi == nullptr) return false;
		auto& gi_ = *gi;
		for (auto& arg : gi_)
		{
			if (arg.name.text == text)
				return true;
		}
		return false;
	}
}

void GenericInst::visit(NamespaceStmt& stmt)
{
	int size_before_pasting = stmt.stmts.size();
	for (size_t i = entry_stmt; i < size_before_pasting; i++) stmt.stmts[i]->accept(*this);
	entry_stmt = size_before_pasting;
}

void GenericInst::visit(IfStmt& if_stmt)
{
	for (auto& p : if_stmt.if_stmts) p->accept(*this);
	for (auto& elif : if_stmt.all_elif_stmts)
	{
		for (auto& p : elif) p->accept(*this);
	}
	for (auto& p : if_stmt.else_stmts) p->accept(*this);
}


void GenericInst::visit(WhileStmt& while_stmt)
{
	for (auto& p : while_stmt.stmts) p->accept(*this);
}

void GenericInst::visit(ForStmt& for_stmt)
{
	for_stmt.decl_stmt->accept(*this);
	for (auto& p : for_stmt.stmts) p->accept(*this);
}

void GenericInst::visit(MatchStmt& match)
{
	for (auto& case_ : match.match_cases)
	{
		for (auto& p : case_.body) p->accept(*this);
	}
}

void GenericInst::visit(FuncDefStmt& stmt)
{
	// The arguments of the function are already instantiated
	for (auto& s : stmt.body) s->accept(*this);
}

void GenericInst::visit(DeclStmt& stmt)
{
	if (stmt.type_spec != nullptr)
	{
		stmt.type_spec->accept(*this);
	}
}

void GenericInst::visit(CollectionStmt& stmt)
{
	generic_params = &stmt.generic_params;
	for (auto& s : stmt.stmts) s->accept(*this);
}

void GenericInst::visit(BaseTypeSpec& bt)
{
	// Base case
	auto defined = scopes.get_type(bt.name.text);
	if (defined == nullptr)
	{
		// Don't try to check generic type parameters passed in 
		if (contains(generic_params, bt.name.text))
		{
			RETURN(false);
		}
		Messages::inst().trigger_4_e1(bt.name);
		RETURN(false);
	}

	// Base case
	if (bt.generic_list.empty())
	{
		// The definition is actually generic
		if (!defined->generic_params.empty())
		{
			std::vector<uptr<TypeSpec>> to_paste;
			// If all generic type parameters are default arguments, we can instantiate something like:
			/*
			struct A<T1=int,T2=int>{}
			struct B{
				A a; <-- We can instantiate this because all parameters are default generic type parameters
			}
			*/
			for (auto& arg : defined->generic_params)
			{
				if (arg.default_type == nullptr)
				{
					Messages::inst().trigger_4_e2(bt.name, arg.name.text);
					RETURN(false);
				}
				arg.default_type->accept(*this);
				to_paste.push_back(arg.default_type->clone());
			}
			//std::reverse(to_paste.begin(), to_paste.end());
			if (!to_paste.empty())
			{
				bt.name.text = get_str(bt.name.text, to_paste);
				bt.generic_list.clear();
			}
			CodePaster paster(*defined, scopes, to_paste, top_level);
			paster.paste();
		}
		RETURN(true);
	}

	/*
	struct A<T,B=int>
	{
	   T a;
	}
	[later...]
	A<int> inst; // <- this is what we want to instantiate
	
	Error when for example A<int,int,int> inst; // Too many type arguments

	Loop through all the generic params of the type definition.
	In the case of A<int>:
	0 : T, int -> put "int" into the list t
	1 : T, _ -> Error when no default argument, but here we have a default arg int 
		-> first instantiate the argument, when it has generic parameters, then put it into t. Here "int"
	now t looks like this: t = [int,int]
	We call the CodePaster to manage the pasting of A with specified type int,int into the AST and Scopes.
	*/
	if (defined->generic_params.size() < bt.generic_list.size())
	{
		Messages::inst().trigger_4_e3(bt.name, bt.as_str(), bt.generic_list.size(), defined->generic_params.size());
		RETURN(false);
	}

	std::vector<uptr<TypeSpec>> to_paste;
	for (int i = 0; i < defined->generic_params.size(); i++)
	{
		if (i >= bt.generic_list.size()) // It's maybe a default argument
		{
			auto& t = defined->generic_params[i];
			// Its not a default argument
			if (t.default_type == nullptr)
			{
				Messages::inst().trigger_4_e4(bt.name, i,defined->name.text);
				RETURN(false); // Dont go any further
			}
			// It is a default argument.
			t.default_type->accept(*this);

			//if (!get(t.default_type)) { RETURN(false); } 
			to_paste.push_back(t.default_type->clone());
			continue;
		}
		auto& given_t = bt.generic_list[i];
		// Don't copy when a generic type parameter was used e.g:
		/*
		struct D<T1>{}
		struct A<T2>{
			D<T2> d; <-- Here we shouldn't instantiate D<T2> because it depends on the generic type parameter T2.
		}
		*/
		if (!get(given_t)) { RETURN(false); }
		to_paste.push_back(given_t->clone());
	}
	//std::reverse(to_paste.begin(), to_paste.end());
	
	if (!to_paste.empty())
	{
		bt.name.text = get_str(bt.name.text, to_paste);
		bt.generic_list.clear();
	}

	CodePaster code_paster(*defined, scopes, to_paste, top_level);
	code_paster.paste();
	RETURN(true);
}

void instantiate_generic(BaseTypeSpec& bt, Scopes& scopes, NamespaceStmt& ns)
{
	GenericInst gi(scopes,ns);
	gi.instantiate(bt);
}

void instantiate_generic_repeat(NamespaceStmt& ns, Scopes& sc)
{
	GenericInst gi(sc, ns);
	size_t n = 0;
	while (ns.stmts.size() > n)
	{
		n = ns.stmts.size();
		ns.accept(gi);
	}
}


//std::pair<bool,GenericInst> instantiate_generic(Scopes& scopes, NamespaceStmt& ns,size_t entry_stmt)
//{
//	GenericInst gi(scopes, ns,entry_stmt);
//	ns.accept(gi);
//	return { gi.get_changed(),gi.get_entry_stmt() };
//}
