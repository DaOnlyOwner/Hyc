#include "DeclarationsCollectorTypes.h"

namespace
{
	template<typename T>
	void add(Scopes& scopes, T& s, const Token& name, const std::string& error_msg)
	{
		bool succ = scopes.add(&s);
		if (!succ)
		{
			auto descr = Error::FromToken(name);
			descr.Message = error_msg;
			Error::SemanticError(descr);
		}
		scopes.descend();
		for (auto& stmt : s.stmts) { stmt->accept(*this); }
	}
}

void DeclarationsCollectorTypes::visit(NamespaceStmt& namespace_stmt)
{
	scopes.descend();
	for (auto& stmt : namespace_stmt.stmts) { stmt->accept(*this); }
}

void DeclarationsCollectorTypes::visit(StructDefStmt& struct_def_stmt)
{
	add(scopes, struct_def_stmt, struct_def_stmt.name, fmt::format("A type with name '{}' has already been defined",struct_def_stmt.name.text));
}

void DeclarationsCollectorTypes::visit(UnionDefStmt& union_def)
{
	add(scopes, union_def, union_def.name, fmt::format("A type with name '{}' has already been defined", union_def.name.text));
}


