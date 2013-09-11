#include "jit.h"

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
#include "llvm/ExecutionEngine/ZMemoryManager.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/LLVMContext.h"
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

#include "llvm/Bitcode/NaCl/NaClReaderWriter.h"  // @LOCALMOD
#include "llvm/IRReader/IRReader.h"  // @LOCALMOD
#include "llvm/Transforms/NaCl.h"
#include "llvm/Analysis/NaCl.h"

using namespace clang;
using namespace clang::driver;
using namespace llvm;

static OwningPtr<llvm::ExecutionEngine> EE;
static OwningPtr<llvm::LLVMContext> Ctx;

extern "C" void shutdown()
{
  EE.reset();
  llvm::llvm_shutdown();
  Ctx.reset();
}

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
  builder.setJITMemoryManager(new ZMemoryManager());
  builder.setOptLevel(llvm::CodeGenOpt::None);
  builder.setMArch("x86-64");

  return builder.create();
}


extern "C"  void* getFunctionPointer(const char *filename, const char *function)
{
  // if no execution engine is present, create new one
  if (!EE)
  {
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    Ctx.reset(new llvm::LLVMContext());

    llvm::sys::Path Path = GetExecutablePath("clang");
    IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
    TextDiagnosticPrinter *DiagClient =
      new TextDiagnosticPrinter(llvm::nulls(), &*DiagOpts);

    IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
    DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);
    Driver TheDriver(Path.str(), llvm::sys::getProcessTriple(), "a.out", Diags);
    TheDriver.setTitle("clang interpreter");

    // FIXME: This is a hack to try to force the driver to do something we can
    // recognize. We need to extend the driver library to support this use model
    // (basically, exactly one input, and the operation mode is hard wired).
    SmallVector<const char *, 16> Args(0);
    // argv[0] = clang - emulate program name
    Args.push_back("clang");
    // argv[1] = file to compile
    Args.push_back(filename);
    Args.push_back("-fsyntax-only");
    OwningPtr<Compilation> C(TheDriver.BuildCompilation(Args));
    if (!C)
      return NULL;

    // FIXME: This is copied from ASTUnit.cpp; simplify and eliminate.

    // We expect to get back exactly one command job, if we didn't something
    // failed. Extract that job from the compilation.
    const driver::JobList &Jobs = C->getJobs();
    if (Jobs.size() != 1 || !isa<driver::Command>(*Jobs.begin())) {
      SmallString<256> Msg;
      llvm::raw_svector_ostream OS(Msg);
      C->PrintJob(OS, C->getJobs(), "; ", true);
      Diags.Report(diag::err_fe_expected_compiler_job) << OS.str();
      return NULL;
    }

    const driver::Command *Cmd = cast<driver::Command>(*Jobs.begin());
    if (llvm::StringRef(Cmd->getCreator().getName()) != "clang") {
      Diags.Report(diag::err_fe_expected_clang_command);
      return NULL;
    }

    // Initialize a compiler invocation object from the clang (-cc1) arguments.
    const driver::ArgStringList &CCArgs = Cmd->getArguments();
    OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
    CompilerInvocation::CreateFromArgs(*CI,
                                       const_cast<const char **>(CCArgs.data()),
                                       const_cast<const char **>(CCArgs.data()) +
                                         CCArgs.size(),
                                       Diags);

    // FIXME: This is copied from cc1_main.cpp; simplify and eliminate.

    // Create a compiler instance to handle the actual work.
    CompilerInstance Clang;
    Clang.setInvocation(CI.take());

    // Create the compilers actual diagnostics engine.
    Clang.createDiagnostics();
    if (!Clang.hasDiagnostics())
      return NULL;

    // Create and execute the frontend to generate an LLVM bitcode module.
    OwningPtr<CodeGenAction> Act(new EmitLLVMOnlyAction(Ctx.get()));
    if (!Clang.ExecuteAction(*Act))
      return NULL;

    if (llvm::Module *Module = Act->takeModule())
    {
      // now let's create MSCJIT instance
      LLVMLinkInMCJIT();
      std::string Error;
      EE.reset(createMCJIT (Module, Error));
      if (!EE) {
        llvm::nulls() << "unable to make execution engine: " << Error << "\n";
        return NULL;
      }
      // compile module, apply memory permissions
      EE->finalizeObject();
      if (EE)
      {
        // retrieve requested function
        Function* F = Module->getFunction(function);
        return EE->getPointerToFunction(F);
      }
    }
  }


  // Execution engine already created
  if (EE)
  {
    // just find damn function
    Function* F = EE->FindFunctionNamed(function);
    if (F) return EE->getPointerToFunction(F);
  }
  return NULL;
}


