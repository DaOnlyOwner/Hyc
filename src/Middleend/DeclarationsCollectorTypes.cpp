#include "DeclarationsCollectorTypes.h"
#include "Messages.h"

namespace
{
	template<typename T>
	void add(DeclarationsCollectorTypes& t, Scopes& scopes, T& s, const Token& name)
	{
		bool succ = scopes.at_root().add(&s);
		scopes.ret();
		if (!succ)
		{
			Messages::inst().trigger_1_e1(name, name.text);
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
	add(*this,scopes, coll_def, coll_def.name);
}

void collect_types(NamespaceStmt& ns,Scopes& sc)
{
	DeclarationsCollectorTypes dc(sc);
	ns.accept(dc); 
}