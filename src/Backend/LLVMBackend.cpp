#include "LLVMBackend.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"
#include <unordered_map>
#include "fmt/color.h"
#include <system_error>
#include "llvm/Support/raw_ostream.h"
#include "Tree.h"
#define NOMINMAX
#include "subprocess.hpp"
#include <random>
#include <limits>
#include <filesystem>

using subprocess::CompletedProcess;
using subprocess::RunBuilder;
using subprocess::PipeOption;

namespace
{
	int emit_ir(const LLVMBackend::CompilerInfo& ci, const llvm::Module& mod)
	{
		std::error_code ec;
		std::unique_ptr<llvm::raw_fd_ostream> dest = nullptr;
		if (!ci.emit_to_stdout)
		{
			dest = std::make_unique<llvm::raw_fd_ostream>(ci.filename_output, ec, llvm::sys::fs::OF_None);
			if (ec.value() != 0)
			{

				fmt::print(fmt::fg(fmt::color::orange_red), "Couldn't open file '{}' with write access\n", ci.filename_output);
				return 1;
			}
		}
		mod.print(ci.emit_to_stdout ? llvm::outs() : *dest, nullptr);
		return 0;
	}

	int emit_obj(const LLVMBackend::CompilerInfo& ci,llvm::TargetMachine* target_machine,llvm::Module& mod)
	{
		std::error_code ec;
		std::unique_ptr<llvm::raw_fd_ostream> dest = nullptr; 
		if (!ci.emit_to_stdout)
		{
			dest = std::make_unique<llvm::raw_fd_ostream>(ci.filename_output, ec, llvm::sys::fs::OF_None);
			if (ec.value() != 0)
			{

				fmt::print(fmt::fg(fmt::color::orange_red), "Couldn't open file '{}' with write access\n", ci.filename_output);
				return 1;
			}
		}
		llvm::legacy::PassManager leg_pm;
		target_machine->addPassesToEmitFile(leg_pm, ci.emit_to_stdout ? llvm::outs() : *dest, nullptr, llvm::CGFT_ObjectFile);
		leg_pm.run(mod);
		return 0;
	}

	int emit_exe(const LLVMBackend::CompilerInfo& ci, llvm::TargetMachine* tm, llvm::Module& mod)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<std::mt19937::result_type> uid(0, std::numeric_limits<std::mt19937::result_type>::max());
		auto rd_num = uid(mt);

		if (ci.emit_to_stdout)
		{
			fmt::print(fmt::fg(fmt::color::orange_red), "Cannot print exe file to stdout, write it to a file instead.");
			return 1;
		}

		auto rev = ci.filename_output;
		std::reverse(rev.begin(), rev.end());
		auto objfile = fmt::format("{}{:x}.o", ci.filename_output.substr(0,rev.size()-rev.find(".")-1),rd_num);
		auto ci_cpy = ci;
		ci_cpy.filename_output = objfile;
		ci_cpy.emit_info = LLVMBackend::CompilerInfo::EmitInfo::EmitObjCode;
		int ec = emit_obj(ci_cpy, tm, mod);
		if (ec != 0) return ec;
		
		try
		{
			subprocess::run({ "clang","-o",ci.filename_output,objfile });
		}
		catch (subprocess::CommandNotFoundError e)
		{
			try
			{
				subprocess::run({ "gcc","-o",ci.filename_output,objfile });
			}
			catch (subprocess::CommandNotFoundError e)
			{
				fmt::print(fmt::fg(fmt::color::orange_red), "Either clang or gcc need to be in PATH for exe output");
				return 1;
			}
		}
		
		std::filesystem::remove(objfile);
		return 0;
	}
}

LLVMBackendInfo& LLVMBackend::init()
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	return be;
}

int LLVMBackend::emit(const CompilerInfo& ci, const std::string& filename)
{
	std::unordered_map<LLVMBackend::CompilerInfo::OptLevel, llvm::PassBuilder::OptimizationLevel> to_llvm =
	{
		{CompilerInfo::OptLevel::O0,llvm::PassBuilder::OptimizationLevel::O0},
		{CompilerInfo::OptLevel::O1,llvm::PassBuilder::OptimizationLevel::O1},
		{CompilerInfo::OptLevel::O2,llvm::PassBuilder::OptimizationLevel::O2},
		{CompilerInfo::OptLevel::O3,llvm::PassBuilder::OptimizationLevel::O3},
		{CompilerInfo::OptLevel::Oz,llvm::PassBuilder::OptimizationLevel::Oz}
	};

	//llvm::InitializeAllTargets();
	//llvm::InitializeAllAsmPrinters();



	std::string target_triple;
	if (ci.target.has_value())
	{
		target_triple = ci.target.value();
	}
	else
	{
		target_triple = llvm::sys::getDefaultTargetTriple();
	}

	std::string error;
	auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);
	if (!target)
	{
		fmt::print(fmt::fg(fmt::color::orange_red),"Error when selecting target: {}\n", error.c_str());
		return 1;
	}

	llvm::LoopAnalysisManager lam;
	llvm::FunctionAnalysisManager fam;
	llvm::CGSCCAnalysisManager cgsccm;
	llvm::ModuleAnalysisManager mam;

	llvm::PassBuilder pb;
	fam.registerPass([&] {return pb.buildDefaultAAPipeline(); });
	pb.registerModuleAnalyses(mam);
	pb.registerCGSCCAnalyses(cgsccm);
	pb.registerFunctionAnalyses(fam);
	pb.registerLoopAnalyses(lam);
	pb.crossRegisterProxies(lam, fam, cgsccm, mam);

	llvm::TargetOptions opt;
	auto RM = llvm::Optional<llvm::Reloc::Model>();
	auto target_machine = target->createTargetMachine(target_triple, "generic", "", opt, RM);
	be.mod.setDataLayout(target_machine->createDataLayout());
	be.mod.setTargetTriple(target_triple);

	ns.accept(stmt_gen);
#ifndef NDEBUG
	auto& outstream = llvm::outs();
	bool b = llvm::verifyModule(be.mod,&outstream);
	if (b)
	{
		fmt::print(fmt::fg(fmt::color::dark_red), "\nCompilerbug: Faulty IR output:\n");
		be.mod.dump();
		return 2;
	}
#endif
	if (ci.opt_lvl != CompilerInfo::OptLevel::O0)
	{
		auto opt_lvl = to_llvm[ci.opt_lvl];
		llvm::ModulePassManager mpm = pb.buildPerModuleDefaultPipeline(opt_lvl);
		mpm.run(be.mod, mam);
	}

	if (ci.emit_info == CompilerInfo::EmitInfo::EmitIRCode)
	{
		return emit_ir(ci, be.mod);
	}

	else if (ci.emit_info == CompilerInfo::EmitInfo::EmitObjCode)
	{
		return emit_obj(ci, target_machine, be.mod);
	}

	else if (ci.emit_info == CompilerInfo::EmitInfo::EmitExe)
	{
		return emit_exe(ci, target_machine, be.mod);
	}

}
