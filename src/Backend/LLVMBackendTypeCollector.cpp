#include "LLVMBackendTypeCollector.h"
#include "llvm/IR/Type.h"

void LLVMBackendTypeCollector::visit(TypeDefStmt& stmt)
{
	if (!stmt.generic_params.empty()) return;
	auto t = llvm::StructType::create(be.context, fmt::format("{}.{}",stmt.get_collection_type(),stmt.name.text));
	stmt.llvm_struct_type = t;
}

void LLVMBackendTypeCollector::visit(FuncDefStmt& def)
{
	//empty
}

void llvm_collect_types(LLVMBackendInfo& be, NamespaceStmt& ns)
{
	LLVMBackendTypeCollector tc(be);
	ns.accept(tc);
}
