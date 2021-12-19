#include "LLVMBackendTypeCollector.h"
#include "llvm/IR/Type.h"

void LLVMBackendTypeCollector::visit(CollectionStmt& stmt)
{
	if (!stmt.generic_params.empty()) return;
	type_hier.add(&stmt);
	auto t = llvm::StructType::create(be.context, stmt.name.text);
	stmt.llvm_struct_type = t;
}

void LLVMBackendTypeCollector::visit(FuncDefStmt& def)
{
}

void llvm_collect_types(LLVMBackendInfo& be, Tree<CollectionStmt*>& type_hier, NamespaceStmt& ns)
{
	LLVMBackendTypeCollector tc(be, type_hier);
	ns.accept(tc);
}