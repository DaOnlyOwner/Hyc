#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

llvm::Type* map_type(const struct Type& from, const class Scopes& scopes, llvm::LLVMContext& ctxt);
llvm::FunctionType* get_function_type(const struct FunctionPointerType& func, const class Scopes& scopes, llvm::LLVMContext& ctxt);