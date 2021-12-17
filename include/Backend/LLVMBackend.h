#pragma once
#include <string>
#include "Ast.h"
#include "Scopes.h"
#include "LLVMBackendStmt.h"
#include <optional>

class LLVMBackend
{
public:
	struct CompilerInfo
	{
		enum class OptLevel
		{
			O0,O1,O2,O3,Oz
		};

		enum class EmitInfo
		{
			EmitObjCode,
			EmitIRCode
		};

		EmitInfo emit_info;
		OptLevel opt_lvl;
		std::optional<std::string> target;
	};

	LLVMBackend(NamespaceStmt& ns, Scopes& scopes)
		:ns(ns), scopes(scopes),be(),stmt_gen(be,scopes){}

	int emit(const CompilerInfo& ci, const std::string& filename);

private:
	CompilerInfo ci;
	NamespaceStmt& ns;
	Scopes& scopes;
	LLVMBackendInfo be;
	LLVMBackendStmt stmt_gen;

};