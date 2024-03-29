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
	void trigger_5_e2(const Token& from, const std::string& proto);
	void trigger_code_paster_e1(const Token& from);

	void trigger_6_e1(const Token& from, const std::string& expr);

	void trigger_6_e2(const Token& from, const std::string& expr);

	void trigger_6_e3(const Token& from, const std::string& t);

	void trigger_6_e4(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e5(const Token& from, const std::string& expr, const std::string& t, const std::string& ptrtype);

	void trigger_6_w1(const Token& from, const std::string& expr);

	void trigger_6_w2(const Token& from, const std::string& expr);

	void trigger_6_w3(const Token& from, const std::string& name);

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

	void trigger_6_e23(const Token& from, const std::string& expr, const std::string& t);

	void trigger_6_w3(const Token& from);

	void trigger_6_e25(const Token& from, const std::string& t1, const std::string& t2);

	void trigger_6_e26(const Token& from, const std::string& expr);

	void trigger_6_e27(const Token& from, const std::string& expr);

	void trigger_6_e28(const Token& from, const std::string& t);

	void trigger_6_e29(const Token& from, const std::string& name, const std::string& t);

	void trigger_6_e30(const Token& from);

	void trigger_6_e31(const Token& from, const std::string& expr);

	void trigger_6_e32(const Token& from, const std::string& ident);

    void trigger_6_e33(const Token& from, const std::string& msg, const std::string& t);
    void trigger_6_e34(const Token& from, const std::string& s);

	void trigger_6_e35(const Token& from, const std::string& t);

	void trigger_6_e36(const Token& from, const std::string& d, const std::string& t);

	void trigger_6_e37(const Token& from);

	void trigger_6_e38(const Token& from);

	void trigger_6_e39(const Token& from);

	void trigger_7_e1(const Token& from, const std::string& coll_name);
	void trigger_8_e1(const Token& from, const std::string& expr);

	void trigger_9_e1(const Token& from, const std::string& embedded_in);

	void trigger_10_e1(const Token& from);

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