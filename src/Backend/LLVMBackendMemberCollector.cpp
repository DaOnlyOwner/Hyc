#include "LLVMBackendMemberCollector.h"
#include "TypeToLLVMType.h"
#include "Messages.h"

void LLVMBackendMemberCollector::visit(DeclStmt& decl)
{
	auto t = map_type(decl.type, scopes, be.context);
	if (decl.type.is_base_type())
	{
		if (!type_hier.add(decl.type.get_base_type(), embedded_in))
		{
			Messages::inst().trigger_9_e1(decl.name, embedded_in->name.text);
		}
	}
	RETURN(t);
}

void LLVMBackendMemberCollector::visit(FuncDefStmt& def)
{
}

void LLVMBackendMemberCollector::visit(CollectionStmt& stmt)
{
	std::vector<llvm::Type*> elements;
	embedded_in = &stmt;
	for (auto& p : stmt.stmts)
	{
		auto t = get(p);
		elements.push_back(t);
	}
	stmt.llvm_struct_type->setBody(elements);
}

void llvm_collect_member(LLVMBackendInfo& be, Tree<CollectionStmt*>& type_hier, Scopes& sc, NamespaceStmt& ns)
{
	LLVMBackendMemberCollector mc(be, type_hier, sc);
	ns.accept(mc);
}
