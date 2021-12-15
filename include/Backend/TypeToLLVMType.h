#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "AtomicType.h"

llvm::Type* map_type(const Type& from);
