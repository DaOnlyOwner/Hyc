#include "GenericInstantiation.h"
#include "DebugPrint.h"

void GenericInst::visit(NamespaceStmt& stmt)
{
	scopes.descend();
	for (auto& s : stmt.stmts) s->accept(*this);
	scopes.go_to_root();
}

void GenericInst::visit(FuncDefStmt& stmt)
{
	scopes.descend();
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

void GenericInst::visit(StructDefStmt& stmt)
{
	scopes.descend();
	for (auto& s : stmt.stmts) s->accept(*this);
}

void GenericInst::visit(UnionDefStmt& stmt)
{
	scopes.descend(); 
	for (auto& s : stmt.stmts) s->accept(*this);
}

void GenericInst::visit(BaseTypeSpec& bt)
{
	// Base case
	if (bt.generic_list.empty())
	{
		if (!scopes.is_type_defined(bt.as_str()))
		{
			auto descr = Error::FromToken(bt.name);
			descr.Message = fmt::format("The type '{}' used in instantiation of the generic type is undefined", bt.name.text);
			Error::SemanticError(descr);
		}
		return;
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
	CollectionStmt* defined = scopes.get_type(bt.name.text);
	if (defined->generic_params.size() < bt.generic_list.size())
	{
		auto descr = Error::FromToken(bt.name);
		descr.Message = fmt::format("The type '{}' was given more generic types for instantiation than the definition specifies.", bt.as_str());
		descr.Hint = fmt::format("In the instantiation the type was given '{}' generic parameters, while the definition has only '{}'", bt.generic_list.size(), defined->generic_params.size());
		Error::SemanticError(descr);
		return;
	}

	std::vector<std::string> to_paste;
	for (int i = 0; i < defined->generic_params.size(); i++)
	{
		if (i >= bt.generic_list.size()) // It's maybe a default argument
		{
			auto& t = defined->generic_params[i];
			// Its not a default argument
			if (t.default_type == nullptr)
			{
				auto descr = Error::FromToken(bt.name);
				std::string num = "";
				if (i == 0) num = "first";
				else if (i == 2) num = "second";
				else if (i == 3) num = "third";
				else if (i == 4) num = "fourth";
				else if (i > 4) num = fmt::format("{}th", i + 1);
				descr.Message = fmt::format("No type specified for the '{}' argument in instantitation of generic type '{}'", num, defined->name.text);
				Error::SemanticError(descr);
				return; // Dont go any further
			}
			t.default_type->accept(*this); // It is a default argument.
			to_paste.push_back(t.default_type->as_str());
			continue;
		}
		auto& given_t = bt.generic_list[i];
		given_t->accept(*this);
		to_paste.push_back(given_t->as_str());
	}

	CodePaster code_paster(*defined, scopes, to_paste, top_level);
	code_paster.paste();
}

void instantiate_generic(BaseTypeSpec& bt, Scopes& scopes, NamespaceStmt& ns)
{
	GenericInst gi(scopes,ns);
	gi.instantiate(bt);
}
