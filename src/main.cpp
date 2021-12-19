#include "Pipeline.h"
#include "cxxopts.hpp"
#include "fmt/color.h"
#include <unordered_map>
#include <string>

std::string get_before_delim(const std::string& str, char delim)
{
	return str.substr(0, str.find(delim));
}

int execute(int argc, char** argv)
{
	cxxopts::Options options("Hyc", "The compiler for Hyc");
	options.add_options()
		("O,opt", "Optimization level (0-3 or z)", cxxopts::value<std::string>()->default_value("1"))
		("e,emit", "Emit (specify 'ir' for IR output, 'obj' for object file output or 'ast' for AST output)", cxxopts::value<std::string>()->default_value("obj"))
		("t,target_triple", "The target triple that should be targeted", cxxopts::value<std::string>())
		("f,file", "The input file", cxxopts::value<std::string>())
		("o,output", "The output file", cxxopts::value<std::string>())
		("stdout", "Write everything to stdout, takes precedence to the -o option")
		("h,help", "Print the help text");
	
	options.parse_positional({ "f" });
	//options.allow_unrecognised_options();
	cxxopts::ParseResult result;
	try
	{
		result = options.parse(argc, argv);
	}

	catch (cxxopts::missing_argument_exception e)
	{
		fmt::print(fmt::fg(fmt::color::orange_red),"{}", e.what());
		return 1;
	}

	if (result.count("h") >= 1)
	{
		fmt::print("{}",options.help());
		return 0;
	}

	if (result.count("O") > 1)
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "Too many arguments given for -O option");
		return 1;
	}
	if (result.count("e") > 1)
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "Too many arguments given for -e (--emit) option");
		return 1;
	}
	
	auto opt = result["O"].as<std::string>();
	if (opt != "0" && opt != "1" && opt != "2" && opt != "3" && opt != "z")
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "Optimization level not correct");
		return 1;
	}
	std::unordered_map<std::string, LLVMBackend::CompilerInfo::OptLevel> to_opt_level
	{
		{"0",LLVMBackend::CompilerInfo::OptLevel::O0},
		{"1",LLVMBackend::CompilerInfo::OptLevel::O1},
		{"2",LLVMBackend::CompilerInfo::OptLevel::O2},
		{"3",LLVMBackend::CompilerInfo::OptLevel::O3},
		{"z",LLVMBackend::CompilerInfo::OptLevel::Oz}
	};
	LLVMBackend::CompilerInfo ci;
	ci.opt_lvl = to_opt_level[opt];
	if (result.count("t") == 0)
	{
		ci.target = std::nullopt;
	}
	else
	{
		ci.target = std::make_optional(result["t"].as<std::string>());
	}

	auto ei = result["e"].as<std::string>();
	if (ei == "ir")
	{
		ci.emit_info = LLVMBackend::CompilerInfo::EmitInfo::EmitIRCode;
	}
	else if (ei == "obj") ci.emit_info = LLVMBackend::CompilerInfo::EmitInfo::EmitObjCode;
	else if (ei == "ast") ci.emit_info = LLVMBackend::CompilerInfo::EmitInfo::EmitAST;
	else
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "Value '{}' for option -e not recognized",ei);
		return 1;
	}

	if (result.count("f") == 0)
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "No file specified", ei);
		return 1;
	}

	auto filename = result["f"].as<std::string>();

	if (result.count("o") == 0)
	{
		std::string out = "";
		if (ci.emit_info == LLVMBackend::CompilerInfo::EmitInfo::EmitIRCode)
		{
			out = fmt::format("{}.ll", get_before_delim(filename, '.'));
		}
		else if (ci.emit_info == LLVMBackend::CompilerInfo::EmitInfo::EmitObjCode)
		{
			out = fmt::format("{}.o", get_before_delim(filename, '.'));
		}
		ci.filename_output = out;
	}

	ci.emit_to_stdout = result.count("stdout") > 0;

	Pipeline pl;
	return pl.build(filename, ci);
}

int main(int argc, char** argv)
{
	return execute(argc, argv);
}

