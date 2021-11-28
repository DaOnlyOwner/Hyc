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

void Messages::semantic_error(const Token& tk, const std::string& msg, const std::string& hint)
{
	auto t = to_comp{ tk.file, tk.lineNo, tk.colNo, msg, hint };
	auto it = semantic_errors.find(t);
	if (it != semantic_errors.end()) return;
	auto descr = Error::FromToken(tk);
	descr.Message = msg;
	descr.Hint = hint;
	Error::SemanticError(descr);
	semantic_errors.insert(t);
}

void Messages::info(const Token& tk, const std::string& msg, const std::string& hint)
{
	auto t = to_comp{ tk.file, tk.lineNo, tk.colNo, msg, hint };
	auto it = semantic_errors.find(t);
	if (it != semantic_errors.end()) return;
	auto descr = Error::FromToken(tk);
	descr.Message = msg;
	descr.Hint = hint;
	Error::Info(descr);
	semantic_errors.insert(t);
}
