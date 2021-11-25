#pragma once

#include "Ast.h"
#include "Scopes.h"
#include "ValueStorage.h"
#include "AtomicType.h"

class DeclarationsCollectorFunctions : public IAstVisitor
{
public:
	DeclarationsCollectorFunctions(Scopes&& scopes)
		:scopes(mv(scopes)){};

	Scopes get_scopes()
	{
		return std::move(scopes);
	}

private:
	Scopes scopes;

	virtual void visit(FuncDefStmt& func_call_def_stmt) override;
};