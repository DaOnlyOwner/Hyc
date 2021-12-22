#include "LLVMBackendMemberCollector.h"
#include "TypeToLLVMType.h"
#include "Messages.h"
#include "llvm/IR/DataLayout.h"
#include <algorithm>

void LLVMBackendMemberCollector::visit(DeclStmt& decl)
{
	auto t = map_type(decl.type, scopes, be.context);
	if (decl.type.is_base_type() && !decl.type.is_predefined(scopes))
	{
		if (!type_hier.add(decl.type.get_base_type(), embedded_in))
		{
			Messages::inst().trigger_9_e1(decl.name, embedded_in->name.text);
		}
	}
	RETURN(t);
}

void LLVMBackendMemberCollector::visit(UnionDeclStmt& udecl)
{
	udecl.decl_stmt->accept(*this);
}

void LLVMBackendMemberCollector::visit(FuncDefStmt& def)
{
}

void LLVMBackendMemberCollector::visit(TypeDefStmt& stmt)
{
	std::vector<llvm::Type*> elements;
	embedded_in = &stmt;
	for (auto& p : stmt.stmts)
	{
		auto t = get(p);
		elements.push_back(t);
	}
	if (stmt.ct == CollectionType::Struct)
	{
		stmt.llvm_struct_type->setBody(elements);
	}
	else if (stmt.ct == CollectionType::Union)
	{
		// Get the one with max size:
		std::vector<llvm::Type*> filtered;
		std::copy_if(elements.begin(), elements.end(), std::back_inserter(filtered), [](const llvm::Type* t) {
			return !t->isVoidTy();
			});
		uint64_t max = 0;
		const llvm::DataLayout& dl = be.mod.getDataLayout();
		if (!filtered.empty())
		{
			max = dl.getTypeStoreSizeInBits(filtered[0]).getValue();
		}

		for (size_t i = 1; i < filtered.size();i++)
		{
			auto e = dl.getTypeStoreSizeInBits(filtered[i]);
			if (max < e.getValue())
			{
				max = e.getValue();
			}
		}

		size_t numOfI8 = max / 8; // max is always a multiple of 8

		if (numOfI8 != 0)
		{
			auto mem0 = llvm::Type::getInt64Ty(be.context);
			auto mem1 = llvm::ArrayType::get(llvm::Type::getInt8Ty(be.context), numOfI8);
			stmt.llvm_struct_type->setBody({ mem0,mem1 });
		}
		else
		{
			auto mem0 = llvm::Type::getInt64Ty(be.context);
			stmt.llvm_struct_type->setBody({ mem0 });
		}
	}
}

void llvm_collect_member(LLVMBackendInfo& be, Tree<TypeDefStmt*>& type_hier, Scopes& sc, NamespaceStmt& ns)
{
	LLVMBackendMemberCollector mc(be, type_hier, sc);
	ns.accept(mc);
}
