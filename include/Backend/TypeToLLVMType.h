#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"

llvm::Type* map_type(const struct Type& from, const class Scopes& scopes, llvm::LLVMContext& ctxt);
