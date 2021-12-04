#include "Messages.h"
#include "fmt/core.h"

Messages& Messages::inst()
{
	static Messages i;
	return i;
}

void Messages::trigger_1_e1(const Token& from, const std::string& str)
{
	semantic_error(from, fmt::format("A type with name '{}' has already been defined", str));
}

void Messages::trigger_2_e1(const Token& from, const std::string& tp, const std::string& arg)
{
	semantic_error(from, fmt::format("The default type of generic type parameter '{}' depends on type '{}' which is also a specified generic type parameter",
		tp, arg));
}

void Messages::trigger_2_e2(const Token& from, const std::string& tp, const std::string& coll_type, const std::string& name)
{
	semantic_error(from, fmt::format("Default type argument of type parameter '{}' of '{}' '{}' is not at the end of the generic type list", tp,
		coll_type, name));
}

void Messages::trigger_2_e3(const Token& from, const std::string& coll)
{
	semantic_error(from, fmt::format("For {} '{}': Two or more generic type parameter have the same name",
		coll,from.text));
}

void Messages::trigger_3_i1(const Token& from, const std::string& func)
{
	info(from, fmt::format("In return type of function '{}'", func));
}

void Messages::trigger_3_i2(const Token& from, const std::string& func_name)
{
	info(from, fmt::format("In type of argument '{}' of function '{}'", from.text, func_name));
}

void Messages::trigger_4_e1(const Token& from)
{
	semantic_error(from, fmt::format("The type '{}' is undefined", from.text));
}

void Messages::trigger_4_e2(const Token& from, const std::string& arg_name)
{
	semantic_error(from, fmt::format("No type parameter given for generic type '{}'", arg_name));
}

void Messages::trigger_4_e3(const Token& from, const std::string& bt, int s1, int s2)
{
	semantic_error(from,
		fmt::format("The type '{}' was given more generic types for instantiation than the definition specifies.", bt),
		fmt::format("In the instantiation the type was given '{}' generic parameters, while the definition has only '{}'", s1, s2));
}

void Messages::trigger_4_e4(const Token& from, int i, const std::string& name)
{
	std::string num = "";
	if (i == 0) num = "first";
	else if (i == 1) num = "second";
	else if (i == 2) num = "third";
	else if (i == 3) num = "fourth";
	else if (i > 3) num = fmt::format("{}th", i + 1);
	std::string msg = fmt::format("No type specified for the {} argument in instantitation of generic type '{}'", num, name);
	semantic_error(from, msg);
}

void Messages::trigger_5_e1(const Token& from, const std::string& proto)
{
	semantic_error(from, fmt::format("The function '{}' is already defined.", proto));
}

void Messages::trigger_code_paster_e1(const Token& from)
{
	semantic_error(from, fmt::format("type '{}': A generic type parameter cannot be generic itself", from.text));
}

void Messages::trigger_6_e1(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("Type of expression '{}' is undefined", expr));
}

void Messages::trigger_6_e2(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("No indirection for non pointer type in expression '{}'", expr));
}

void Messages::trigger_6_e3(const Token& from, const std::string& t)
{
	semantic_error(from, fmt::format("No overloaded operator '{}' defined for type '{}'", from.text, t));
}

void Messages::trigger_6_e4(const Token& from,const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("The operator '{}' is not specified for types '{}' and '{}'", t1, t2));
}

void Messages::trigger_6_e5(const Token& from)
{
	semantic_error(from, "Only '0' (nullpointer) and other pointer values may be assigned to pointers");
}

void Messages::trigger_6_w1(const Token& from, const std::string& expr)
{
	warning(from, fmt::format("Casting type of expression '{}' to signed numeric", expr));
}

void Messages::trigger_6_e6(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("For atomic types: The types of operator '{}' must be integer, not '{}' and '{}'", 
		from.text, t1,t2));
}

void Messages::trigger_6_e7(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("For atomic types: The types of operator '{}' must be bool, not '{}' and '{}'",
		from.text, t1, t2));
}

void Messages::trigger_6_e8(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("No operator '{}' defined for types '{}' and '{}'", from.text, t1, t2));
}

void Messages::trigger_6_e9(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("The type of expression '{}' is void and cannot be used in a binary operator",
		expr));
}

void Messages::trigger_6_e10(const Token& from)
{
	semantic_error(from, fmt::format("Can only infer a type when the operator is '=' or '#', not '{}'", from.text));
}

void Messages::trigger_6_e11(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("The type of the right hand side (here expression '{}') cannot be inferred"
		, expr));
}

void Messages::trigger_6_e12(const Token& from, const std::string& t, const std::string& member)
{
	semantic_error(from, fmt::format("Type '{}' has no member called '{}'", t,member));
}

void Messages::trigger_6_e13(const Token& from, const std::string& t)
{
	semantic_error(from, fmt::format("Type '{}' has no members", t));
}

void Messages::trigger_6_e14(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("The right hand side of operator '{}' needs to be an identifier, but is '{}'",
		from.text, expr));
}

void Messages::trigger_6_e15(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("Can only infer type when the right hand side is an identifier, here is '{}'",
		expr));
}


void Messages::semantic_error(const Token& tk, const std::string& msg, const std::string& hint)
{
	auto t = to_comp{ tk.file, tk.lineNo, tk.colNo, msg, hint };
	auto it = messages.find(t);
	if (it != messages.end()) return;
	auto descr = Error::FromToken(tk);
	descr.Message = msg;
	descr.Hint = hint;
	Error::SemanticError(descr);
	messages.insert(t);
}

void Messages::info(const Token& tk, const std::string& msg, const std::string& hint)
{
	auto t = to_comp{ tk.file, tk.lineNo, tk.colNo, msg, hint };
	auto it = messages.find(t);
	if (it != messages.end()) return;
	auto descr = Error::FromToken(tk);
	descr.Message = msg;
	descr.Hint = hint;
	Error::Info(descr);
	messages.insert(t);
}

void Messages::warning(const Token& tk, const std::string& msg, const std::string& hint)
{
	auto t = to_comp{ tk.file, tk.lineNo, tk.colNo, msg, hint };
	auto it = messages.find(t);
	if (it != messages.end()) return;
	auto descr = Error::FromToken(tk);
	descr.Message = msg;
	descr.Hint = hint;
	Error::Warning(descr);
	messages.insert(t);
}
