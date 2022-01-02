#pragma once
#include <string>

std::string mangle(const struct FuncDefStmt& def);
std::string mangle(const struct FuncDeclStmt& decl);
std::string mangle(const struct DelOpExpr& doe);