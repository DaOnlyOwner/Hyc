#pragma once

#include "Token.h"
#include "DebugPrint.h"
#include <string>
#include <set>
#include <tuple>
#include <stdexcept>

#define NOT_IMPLEMENTED throw std::runtime_error("feature currently not implemented");

class Messages
{
public:
	static Messages& inst();
	Messages(const Messages&) = delete;
	Messages(Messages&&) = delete;
	Messages& operator=(const Messages&) = delete;
	Messages& operator=(Messages&&) = delete;

	void trigger_1_e1(const Token& from, const std::string& str);
	void trigger_2_e1(const Token& from, const std::string& tp, const std::string& arg);
	void trigger_2_e2(const Token& from, const std::string& tp, const std::string& coll_type, const std::string& name);
	void trigger_2_e3(const Token& from, const std::string& coll);
	void trigger_3_i1(const Token& from, const std::string& func);
	void trigger_3_i2(const Token& from, const std::string& func_name);
	void trigger_4_e1(const Token& from);
	void trigger_4_e2(const Token& from, const std::string& arg_name);
	void trigger_4_e3(const Token& from, const std::string& bt, int s1, int s2);
	void trigger_4_e4(const Token& from, int i, const std::string& name);
	void trigger_5_e1(const Token& from, const std::string& proto);
	void trigger_code_paster_e1(const Token& from);

	void trigger_6_e1(const Token& from, const std::string& expr);

	void trigger_6_e2(const Token& from, const std::string& expr);

	void trigger_6_e3(const Token& from, const std::string& t);

	void trigger_6_e4(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e5(const Token& from, const std::string& expr, const std::string& t, const std::string& ptrtype);

	void trigger_6_w1(const Token& from, const std::string& expr);

	void trigger_6_e6(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e6_2(const Token& from, const std::string& t1);

	void trigger_6_e7(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e8(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e9(const Token& from, const std::string& expr);

	void trigger_6_e10(const Token& from);

	void trigger_6_e11(const Token& from, const std::string& expr);

	void trigger_6_e12(const Token& from, const std::string& t, const std::string& member);

	void trigger_6_e13(const Token& from, const std::string& t);

	void trigger_6_e14(const Token& from, const std::string& expr);

	void trigger_6_e15(const Token& from, const std::string& expr);

	void trigger_6_e16(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e17(const Token& from);

	void trigger_6_e18(const Token& token, const std::string& t, const std::string& expr);

	void trigger_6_e19(const Token& from);
	void trigger_6_e20(const Token& from, const std::string& expr);
	void trigger_6_e21(const Token& from, const std::string& args);
	void trigger_6_e22(const Token& from, const std::string& expr, const std::string& t);


	void trigger_7_e1(const Token& from, const std::string& coll_name);
	void trigger_8_e1(const Token& from, const std::string& expr);

private:
	Messages() = default;

	void semantic_error(const Token& tk, const std::string& msg, const std::string& hint = "");
	void info(const Token& tk, const std::string& msg, const std::string& hint = "");
	void warning(const Token& tk, const std::string& msg, const std::string& hint = "");
	struct to_comp
	{
		std::string file;
		size_t line;
		size_t col;
		std::string msg;
		std::string hint;

	};
	friend bool operator<(const to_comp& lh, const to_comp& rh)
	{
		auto a1 = std::tie(lh.file, lh.line, lh.col, lh.msg, lh.hint);
		auto a2 = std::tie(rh.file, rh.line, rh.col, rh.msg, rh.hint);
		return a1 < a2;
	}


	std::set<to_comp> messages;
};