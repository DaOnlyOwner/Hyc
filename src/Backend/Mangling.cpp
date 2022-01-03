#include "Mangling.h"
#include "Ast.h"
#include "fmt/core.h"
#include <algorithm>

std::string mangle(const FuncDefStmt& def, bool mangle_op)
{
	if (def.is_operator && !mangle_op)
	{
		return def.decl->name.text;
	}
	return mangle(*def.decl);
}

std::string mangle(const FuncDeclStmt& decl)
{
	if (decl.name.text == "main") return "main";
	std::string out = decl.name.text;

	for (int i = 0; i < decl.arg_list.size(); i++)
	{
		auto& arg = decl.arg_list[i];
		out += fmt::format("_{}", arg->type.as_str_for_mangling());
	}

	if (decl.arg_list.empty())
	{
		out += "_void";
	}

	return out;
}

std::string mangle(const DelOpExpr& doe)
{
	std::string out = "del";

	auto& arg = doe.expr->sem_type.with_pointer();
	out += fmt::format("_{}", arg.as_str_for_mangling());
	return out;
}

std::string mangle(const BinOpExpr& bin)
{
	std::string out = bin.op.text;
	return fmt::format("{}_{}_{}", out, bin.lh->sem_type.with_pointer().as_str_for_mangling(),
		bin.rh->sem_type.with_pointer().as_str_for_mangling());
}
