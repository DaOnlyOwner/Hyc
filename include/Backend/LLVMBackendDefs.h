#pragma once
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"


struct LLVMBackendInfo
{
	LLVMBackendInfo()
		:context(),mod("global",context),builder(context){}
	llvm::LLVMContext context;
	llvm::Module mod;
	llvm::IRBuilder<> builder;
};

struct StackAllocatedMem
{
	std::unordered_map<std::string, llvm::Value*> global_mem;
	std::unordered_map<std::string, llvm::Value*> function_stack;

	void enter_function(llvm::Function* func)
	{
		function_stack.clear();
		for (auto& arg : func->args())
		{
			function_stack[arg.getName().str()] = &arg;
		}
	}
};