#pragma once
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"

#define UNION_TAG 0
#define UNION_FIELD 1

struct LLVMBackendInfo
{
	LLVMBackendInfo()
		:context(),mod("global",context),builder(context){}
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};


inline llvm::ConstantInt* create_ci(uint64_t val, llvm::LLVMContext& ctxt)
{
	return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctxt), val);
}

inline llvm::Value* create_casted_union_field(Scopes& scopes,UnionDeclStmt* udecl, LLVMBackendInfo& be, llvm::Value* lhs)
{
	auto t = udecl->decl_stmt->type;
	t.promote_pointer();
	auto llvm_t = map_type(t, scopes, be.context);
	auto untyped_field = be.builder.CreateStructGEP(lhs, UNION_FIELD);
	return be.builder.CreateBitCast(untyped_field, llvm_t);
}