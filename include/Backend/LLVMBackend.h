#pragma once
#include <string>
#include "Ast.h"
#include "Scopes.h"
#include "LLVMBackendStmt.h"

class LLVMBackend
{
public:
	struct CompilerInfo
	{
		enum class OptLevel
		{
			O1,O2,O3
		};

		enum class EmitInfo
		{
			EmitObjCode,
			EmitIRCode
		};

		std::string filename;
		EmitInfo emit_info;
		OptLevel opt_lvl;
	};

	LLVMBackend(NamespaceStmt& ns, Scopes& scopes)
		:ns(ns), scopes(scopes), be{llvm::LLVMContext()}{}

private:
	CompilerInfo ci;
	NamespaceStmt& ns;
	Scopes& scopes;
	LLVMBackendInfo be;

};