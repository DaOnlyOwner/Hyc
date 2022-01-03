#pragma once
#include <string>

std::string mangle(const struct FuncDefStmt& def, bool mangle_op = false);
std::string mangle(const struct FuncDeclStmt& decl);
std::string mangle(const struct DelOpExpr& doe);
std::string mangle(const struct BinOpExpr& bin);