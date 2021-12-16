#include "Pipeline.h"
#include "cxxopts.hpp"
#include "fmt/color.h"

// TODO: Correctness: Handle user errors

int main(int argc, char** argv)
{
	cxxopts::Options options("Hyc", "The compiler for Hyc");
	options.add_options()
		("O", "Optimization level (0-3)", cxxopts::value<int>()->default_value("1"))
		("e,emit", "Emit (specify 'ir' for IR output or 'obj' for object file output)", cxxopts::value<std::string>()->default_value("obj"));
	auto result = options.parse(argc, argv);
	if (result.count("O") > 1)
	{
		fmt::print(fmt::fg(fmt::color::dark_red), "Too many arguments given for -O option");
		return 1;
	}
	if (result.count("e") > 1)
	{
		fmt::print(fmt::fg(fmt::color::orange_red), "Too many arguments given for -e (--emit) option");
		return 1;
	}
}
