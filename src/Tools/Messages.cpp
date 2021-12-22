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
	semantic_error(from, fmt::format("No indirection for non pointer type, in expression '{}'", expr),"If you are trying to dereference a function pointer, you can't do that, calling a function pointer looks like a regular function call.");
}

void Messages::trigger_6_e3(const Token& from, const std::string& t)
{
	semantic_error(from, fmt::format("No overloaded operator '{}' defined for type '{}'", from.text, t));
}

void Messages::trigger_6_e4(const Token& from,const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("The operator '{}' is not specified for types '{}' and '{}'", from.text, t1, t2));
}

// TODO: Null pointers syntax (nullptr).
void Messages::trigger_6_e5(const Token& from, const std::string& expr, const std::string& t, const std::string& ptrtype)
{
	semantic_error(from, fmt::format("Only '0' (nullpointer) and other pointer values may be assigned to pointers, here a variable of type '{}' was assigned '{}' which has type '{}'",ptrtype, expr,t));
}

void Messages::trigger_6_w1(const Token& from, const std::string& expr)
{
	warning(from, fmt::format("Casting type of expression '{}' to signed integer", expr));
}

void Messages::trigger_6_w2(const Token& from, const std::string& expr)
{
	warning(from, fmt::format("Casting type of expression '{}' to unsigned integer", expr));
}

void Messages::trigger_6_w3(const Token& from, const std::string& name)
{
	warning(from, fmt::format("Match statement has no case for '{}'", name));
}

void Messages::trigger_6_e6(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("For atomic types: The types of operator '{}' must be integer, not '{}' and '{}'", 
		from.text, t1,t2));
}

void Messages::trigger_6_e6_2(const Token& from, const std::string& t1)
{
	semantic_error(from, fmt::format("For atomic types: The types of operator '{}' must be integer, not '{}'",
		from.text, t1));
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
	semantic_error(from, fmt::format("The type of expression '{}' is void and cannot be used as an operand",
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
	semantic_error(from, fmt::format("Can only infer type when the left hand side is an identifier, here is '{}'",
		expr));
}

void Messages::trigger_6_e16(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("Cannot assign type '{}' to type '{}'",
		t1,t2));
}

void Messages::trigger_6_e17(const Token& from)
{
	semantic_error(from, fmt::format("Variable with name '{}' is already defined in this scope or above",
		from.text));
}

void Messages::trigger_6_e18(const Token& from, const std::string& t, const std::string& expr)
{
	semantic_error(from, fmt::format("Wrong type '{}' of expression '{}' for operator '!'", t, expr),"Operator '!' needs type 'bool'");
}

void Messages::trigger_6_e19(const Token& from)
{
	semantic_error(from, fmt::format("Variable '{}' is undefined",from.text));
}

void Messages::trigger_6_e20(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("Cannot deduce type '{}' in function call '{}'", from.text,expr));
}

void Messages::trigger_6_e21(const Token& from, const std::string& args)
{
	semantic_error(from, fmt::format("No function '{}' defined with argument types '{}'", from.text, args));
}

void Messages::trigger_6_e22(const Token& from, const std::string& expr, const std::string& t)
{
	semantic_error(from, fmt::format("The expression '{}' needs to be of bool, but has type '{}'", expr, t));
}

void Messages::trigger_6_e23(const Token& from, const std::string& expr, const std::string& t)
{
	semantic_error(from, fmt::format("Cannot dereference a non pointer (expression '{}' has type '{}')", expr,t));
}

void Messages::trigger_6_w3(const Token& from)
{
	warning(from, fmt::format("Non void function '{}' has no return statement", from.text));
}

void Messages::trigger_6_e25(const Token& from, const std::string& t1, const std::string& t2)
{
	semantic_error(from, fmt::format("Returned type '{}' does not match the function return type '{}'", t1, t2));
}

void Messages::trigger_6_e26(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("Cannot take subscript of a non pointer/array, in expression '{}'", expr));
}

void Messages::trigger_6_e27(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("The type of the expression '{}' within the array subscript is not an unsigned integer",expr));
}

void Messages::trigger_6_e28(const Token& from, const std::string& t)
{
	semantic_error(from, fmt::format("The type of '{}' is not a union, but is '{}'", from.text, t));
}

void Messages::trigger_6_e29(const Token& from, const std::string& name, const std::string& t)
{
	semantic_error(from, fmt::format("Cannot create a pointer to untyped member '{}' of union '{}'", from.text, t));
}

void Messages::trigger_7_e1(const Token& from, const std::string& coll_name)
{
	semantic_error(from, fmt::format("Variable with name '{}' is already defined in collection '{}'", from.text, coll_name));
}

void Messages::trigger_8_e1(const Token& from, const std::string& expr)
{
	semantic_error(from, fmt::format("Cannot assign to a non lvalue expression: '{}'", expr));
}

void Messages::trigger_9_e1(const Token& from, const std::string& embedded_in)
{
	semantic_error(from, fmt::format("Embedding '{}' in '{}' creates a cyclic dependency", from.text, embedded_in));
}

void Messages::trigger_10_e1(const Token& from)
{
	semantic_error(from, "Last type specifier needs to be a pointer");
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
