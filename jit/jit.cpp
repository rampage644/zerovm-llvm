//===-- examples/clang-interpreter/main.cpp - Clang C Interpreter Example -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/FrontendTool/Utils.h"
#include "llvm/PassManager.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetMachine.h"

#include "dump_listener.h"

#include "llvm/Bitcode/NaCl/NaClReaderWriter.h"  // @LOCALMOD
#include "llvm/IRReader/IRReader.h"  // @LOCALMOD
#include "llvm/Transforms/NaCl.h"
#include "llvm/Analysis/NaCl.h"

using namespace clang;
using namespace clang::driver;

// This function isn't referenced outside its translation unit, but it
// can't use the "static" keyword because its address is used for
// GetMainExecutable (since some platforms don't support taking the
// address of main, and some platforms can't implement GetMainExecutable
// without being given the address of a function in the main executable).
llvm::sys::Path GetExecutablePath(const char *Argv0) {
  // This just needs to be some symbol in the binary; C++ doesn't
  // allow taking the address of ::main however.
  void *MainAddr = (void*) (intptr_t) GetExecutablePath;
  return llvm::sys::Path::GetMainExecutable(Argv0, MainAddr);
}
llvm::ExecutionEngine* createMCJIT(llvm::Module* Module, std::string& Error) {
  EngineBuilder builder(Module);
  builder.setErrorStr(&Error);
  builder.setEngineKind(EngineKind::JIT);
  builder.setUseMCJIT(true);
  builder.setJITMemoryManager(new SectionMemoryManager());
  builder.setOptLevel(llvm::CodeGenOpt::None);
  builder.setMArch("x86-64");

  return builder.create();
}

static int Execute(llvm::Module *Mod, char * const *envp) {
  llvm::InitializeNativeTarget();
  LLVMLinkInMCJIT();

  std::string Error;
  OwningPtr<llvm::ExecutionEngine> EE(
    createMCJIT (Mod, Error));
  if (!EE) {  
    llvm::errs() << "unable to make execution engine: " << Error << "\n";
    return 255;
  }

  CustomJITEventListener listener;
  EE->RegisterJITEventListener(&listener);

  llvm::Function *EntryFn = Mod->getFunction("main");
  if (!EntryFn) {
    llvm::errs() << "'main' function not found in module.\n";
    return 255;
  }

  // FIXME: Support passing arguments.
  std::vector<std::string> Args;
  Args.push_back(Mod->getModuleIdentifier());

  EE->finalizeObject();
  return EE->runFunctionAsMain(EntryFn, Args, envp);
}
static int Compile(llvm::Module* m) {
  llvm::InitializeNativeTarget();

  std::string Error;
  llvm::PassManager PM;
  std::string MArch = "x86-64";
  std::string TargetTriple = "x86_64-pc-nacl";
  llvm::Triple TheTriple;

  m->setTargetTriple(llvm::Triple::normalize(TargetTriple));
  TheTriple = Triple(m->getTargetTriple());

  const llvm::Target* Trgt = llvm::TargetRegistry::lookupTarget (
                               MArch, TheTriple, Error);
  if (!Trgt) {
      llvm::errs () << Error;
      return 1;
  }
  // Package up features to be passed to target/subtarget
  std::string FeaturesStr;
  if (MAttrs.size()) {
    llvm::SubtargetFeatures Features;
    for (unsigned i = 0; i != MAttrs.size(); ++i)
      Features.AddFeature(MAttrs[i]);
    FeaturesStr = Features.getString();
  }
  llvm::TargetOptions Options;
  llvm::TargetMachine* TM = Trgt->createTargetMachine (
              llvm::sys::getDefaultTargetTriple (),
              MCPU, FeaturesStr,
              Options,
              RelocModel, CMModel,
              CodeGenOpt::None);
  if (!TM) {
      llvm::errs () << "Can't allocate target machine!";
      return 1;
  }

  llvm::outs() << "TM: " <<
                  llvm::sys::getDefaultTargetTriple() << " " <<
                  Trgt->getName() << " " << Trgt->getShortDescription() << " " <<
                  TM->getTargetCPU() << " " << TM->getTargetTriple() << "\n";

//  PNaClABIErrorReporter ABIErrorReporter; // @LOCALMOD
//  FunctionPass *FunctionVerifyPass = NULL;
//  FunctionVerifyPass = createPNaClABIVerifyFunctionsPass(&ABIErrorReporter);
//  PM.add(FunctionVerifyPass);

//  // Add the intrinsic resolution pass. It assumes ABI-conformant code.
//  PM.add(createResolvePNaClIntrinsicsPass());

  TM->addAnalysisPasses(PM);
  PM.add(new DataLayout(*TM->getDataLayout()));

  llvm::raw_fd_ostream OS ("/dev/main.o", Error, llvm::raw_fd_ostream::F_Binary);

  if (OS.has_error()) {
    llvm::errs() << Error << '\n';
    return 1;
  }
  // Turn the machine code intermediate representation into bytes in memory
  // that may be executed.
//  llvm::MCContext* Ctx = 0;
//  if (TM->addPassesToEmitMC(PM, Ctx, of, false)) {
//    report_fatal_error("Target does not support MC emission!");
//  }

  llvm::formatted_raw_ostream FOS(OS);
  if (TM->addPassesToEmitFile(PM, FOS, TargetMachine::CGFT_ObjectFile)) {
    errs() << ": target does not support generation of this"
           << " file type!\n";
    return 1;
  }

  // Initialize passes.
  PM.run(*m);
  // Flush the output buffer to get the generated code into memory
  FOS.flush();
  OS.close();
  return 0;
}

static int WriteBitcode(llvm::Module* Module) {
  std::string Error;
  llvm::raw_fd_ostream OS ("/dev/main.o", Error, llvm::raw_fd_ostream::F_Binary);

  if (OS.has_error()) {
    llvm::errs() << Error << '\n';
    return 1;
  }

//  llvm::NaClWriteBitcodeToFile(Module, OS);
  llvm::WriteBitcodeToFile(Module, OS);

  OS.close();
  return 0;
}

int main(int argc, const char **argv, char * const *envp) {
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
//  llvm::InitializeAllAsmParsers();

  void *MainAddr = (void*) (intptr_t) GetExecutablePath;
  llvm::sys::Path Path = GetExecutablePath(argv[0]);
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);

  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);
  Driver TheDriver(Path.str(), llvm::sys::getProcessTriple(), "a.out", Diags);
  TheDriver.setTitle("clang interpreter");

  // FIXME: This is a hack to try to force the driver to do something we can
  // recognize. We need to extend the driver library to support this use model
  // (basically, exactly one input, and the operation mode is hard wired).
  SmallVector<const char *, 16> Args(argv, argv + argc);
  Args.push_back("-fsyntax-only");
  OwningPtr<Compilation> C(TheDriver.BuildCompilation(Args));
  if (!C)
    return 0;

  // FIXME: This is copied from ASTUnit.cpp; simplify and eliminate.

  // We expect to get back exactly one command job, if we didn't something
  // failed. Extract that job from the compilation.
  const driver::JobList &Jobs = C->getJobs();
  if (Jobs.size() != 1 || !isa<driver::Command>(*Jobs.begin())) {
    SmallString<256> Msg;
    llvm::raw_svector_ostream OS(Msg);
    C->PrintJob(OS, C->getJobs(), "; ", true);
    Diags.Report(diag::err_fe_expected_compiler_job) << OS.str();
    return 1;
  }

  const driver::Command *Cmd = cast<driver::Command>(*Jobs.begin());
  if (llvm::StringRef(Cmd->getCreator().getName()) != "clang") {
    Diags.Report(diag::err_fe_expected_clang_command);
    return 1;
  }

  // Initialize a compiler invocation object from the clang (-cc1) arguments.
  const driver::ArgStringList &CCArgs = Cmd->getArguments();
  OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  CompilerInvocation::CreateFromArgs(*CI,
                                     const_cast<const char **>(CCArgs.data()),
                                     const_cast<const char **>(CCArgs.data()) +
                                       CCArgs.size(),
                                     Diags);

  // Show the invocation, with -v.
  if (CI->getHeaderSearchOpts().Verbose) {
    llvm::errs() << "clang invocation:\n";
    C->PrintJob(llvm::errs(), C->getJobs(), "\n", true);
    llvm::errs() << "\n";
  }

  // FIXME: This is copied from cc1_main.cpp; simplify and eliminate.

  // Create a compiler instance to handle the actual work.
  CompilerInstance Clang;
  Clang.setInvocation(CI.take());

  // Create the compilers actual diagnostics engine.
  Clang.createDiagnostics();
  if (!Clang.hasDiagnostics())
    return 1;

  // Infer the builtin include path if unspecified.
  if (Clang.getHeaderSearchOpts().UseBuiltinIncludes &&
      Clang.getHeaderSearchOpts().ResourceDir.empty())
    Clang.getHeaderSearchOpts().ResourceDir =
      CompilerInvocation::GetResourcesPath(argv[0], MainAddr);

#ifndef NDEBUG
  llvm::DebugFlag = false;
#endif
  // Create and execute the frontend to generate an LLVM bitcode module.
  OwningPtr<CodeGenAction> Act(new EmitBCAction());
  if (!Clang.ExecuteAction(*Act))
    return 1;

  int r = 0;
  if (llvm::Module *Module = Act->takeModule()) {
    r = Execute (Module, envp);
//    Compile(Module);
//      WriteBitcode(Module);
  }
  // Shutdown.
  llvm::llvm_shutdown();

  return r;
}
