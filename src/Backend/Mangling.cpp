#include "Mangling.h"
#include "Ast.h"
#include "fmt/core.h"
#include <algorithm>

std::string mangle(const FuncDeclStmt& decl)
{
	if (decl.name.text == "main") return "main";
	std::string out = decl.name.text;
	
	for (int i = 0; i<decl.arg_list.size(); i++)
	{
		auto& arg = decl.arg_list[i];
		out += fmt::format("_{}",arg->type.as_str_for_mangling());
	}

	if (decl.arg_list.empty())
	{
		out += "_void";
	}

	return out;
}
