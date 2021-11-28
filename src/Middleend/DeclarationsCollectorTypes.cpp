#include "DeclarationsCollectorTypes.h"

namespace
{
	template<typename T>
	void add(DeclarationsCollectorTypes& t, Scopes& scopes, T& s, const Token& name, const std::string& error_msg)
	{
		bool succ = scopes.at_root().add(&s);
		scopes.ret();
		if (!succ)
		{
			auto descr = Error::FromToken(name);
			descr.Message = error_msg;
			Error::SemanticError(descr);
		}
		scopes.descend();
		for (auto& stmt : s.stmts) { stmt->accept(t); }
	}
}

void DeclarationsCollectorTypes::visit(NamespaceStmt& namespace_stmt)
{
	scopes.descend();
	for (auto& stmt : namespace_stmt.stmts) { stmt->accept(*this); }
}

void DeclarationsCollectorTypes::visit(CollectionStmt& coll_def)
{
	add(*this,scopes, coll_def, coll_def.name, fmt::format("A type with name '{}' has already been defined", coll_def.name.text));
}

void collect_types(NamespaceStmt& ns,Scopes& sc)
{
	DeclarationsCollectorTypes dc(sc);
	ns.accept(dc); 
}
