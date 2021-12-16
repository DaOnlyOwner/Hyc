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

namespace
{
	std::string get_before_delim(const std::string& str, char delim)
	{
		return str.substr(0, str.find(delim));
	}
}


int LLVMBackend::emit(const CompilerInfo& ci, const std::string& filename)
{
	std::unordered_map<LLVMBackend::CompilerInfo::OptLevel, llvm::PassBuilder::OptimizationLevel> to_llvm =
	{
		{CompilerInfo::OptLevel::O0,llvm::PassBuilder::OptimizationLevel::O0},
		{CompilerInfo::OptLevel::O1,llvm::PassBuilder::OptimizationLevel::O1},
		{CompilerInfo::OptLevel::O2,llvm::PassBuilder::OptimizationLevel::O2},
		{CompilerInfo::OptLevel::O3,llvm::PassBuilder::OptimizationLevel::O3}
	};
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();

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
		fmt::print(fmt::fg(fmt::color::red),"Error when selecting target: {}\n", error.c_str());
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

	llvm::ModulePassManager mpm = pb.buildPerModuleDefaultPipeline(to_llvm[ci.opt_lvl]);
	//mpm.addPass()
	ns.accept(stmt_gen);
#ifndef NDEBUG
	bool b = llvm::verifyModule(be.mod);
	if (!b)
	{
		fmt::print(fmt::fg(fmt::color::dark_red), "Compilerbug, aborting\n");
	}
#endif
	mpm.run(be.mod, mam);

	std::error_code ec;
	if (ec)
	{
		fmt::print(fmt::fg(fmt::color::red), "Couldn't open file '{}' with write access\n", filename);
	}
	if (ci.emit_info == CompilerInfo::EmitInfo::EmitIRCode)
	{
		auto llfilename = fmt::format("{}.ll", get_before_delim(filename, '.'));
		llvm::raw_fd_ostream dest(llfilename, ec, llvm::sys::fs::OF_None);
		be.mod.print(dest, nullptr);
		return 0;
	}

	else if (ci.emit_info == CompilerInfo::EmitInfo::EmitObjCode)
	{
		auto ofilename = fmt::format("{}.o", get_before_delim(filename, '.'));
		llvm::raw_fd_ostream dest(ofilename, ec, llvm::sys::fs::OF_None);
		llvm::legacy::PassManager leg_pm;
		target_machine->addPassesToEmitFile(leg_pm, dest, nullptr, llvm::CGFT_ObjectFile);
		leg_pm.run(be.mod);
		return 0;
	}
}
