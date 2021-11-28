#pragma once

#include "Token.h"
#include "DebugPrint.h"
#include <string>
#include <set>
#include <tuple>

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

private:
	Messages() = default;

	void semantic_error(const Token& tk, const std::string& msg, const std::string& hint = "");
	void info(const Token& tk, const std::string& msg, const std::string& hint = "");
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


	std::set<to_comp> semantic_errors;
};