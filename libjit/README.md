libjit
=========================

## Getting the source

`git clone https://github.com/rampage644/zerovm-llvm`

`cd zerovm-llvm`

`cd libjit`

`make`

## Overview

Small library to compile-and-run (JIT or AOT) C/C++ source files under **ZeroVM 
sessions**.

## Usage

It's written in c++ with heavy usage of libLLVM and libClang.

Copy jit.h header file into your project and link with libjit.a.

Currently library exports only one function:

    void* getFunctionPointer(const char* filename,
                             const char* function);

Routine gets filename and function defined in it, compiles whole module,
applies read-execute permissions and returns address to function. If something
goes wrong it returns NULL. Client has to cast void* to function pointer.

## Example

`cat main.cpp`:

    int sum (int a, int b) {
      return a + b;
    }

Use library with given main.cpp as follows:

    void* ptr = getFunctionPointer("main.cpp", "sum");
    ASSERT_NE(ptr, (void*)NULL);
    typedef int (*fptr)(int, int);
    fptr function = (fptr) ptr;
    ASSERT_NE(function, (void*)NULL);
    // function = two int's sum
    EXPECT_EQ(function(0, 0), 0);
    EXPECT_EQ(function(1, 5), 6);
    EXPECT_EQ(function(5, -9), -4);

See more examples in tests.cpp.

## Limitations

1. Only one source file is supported. First used source file will be compiled.
  Subsequent calls getFunctionPointer will try to find given function in first 
  used source file, filename would be ignored.

## Dependencies         

libjit relies on _Clang_ source code parsing, _LLVM_ code generation and _libzdl_ 
symbol resolution.

Client should link with following libs:

* libjit 
* clangFrontendTool 
* clangFrontend 
* clangSerialization 
* clangDriver 
* clangCodeGen 
* clangParse 
* clangSema 
* clangStaticAnalyzerFrontend 
* clangStaticAnalyzerCheckers
* clangStaticAnalyzerCore 
* clangAnalysis 
* clangRewriteFrontend 
* clangRewriteCore
* clangARCMigrate
* clangEdit 
* clangAST 
* clangLex 
* clangBasic 
* LLVMIRReader 
* LLVMInstrumentation
* LLVMAsmParser 
* LLVMLinker 
* LLVMipo 
* LLVMVectorize 
* LLVMBitWriter 
* LLVMBitReader
* LLVMX86Desc 
* LLVMX86Info 
* LLVMX86AsmPrinter 
* LLVMX86Utils
* LLVMSelectionDAG 
* LLVMAsmPrinter 
* LLVMMCParser 
* LLVMInterpreter 
* LLVMNaClBitWriter 
* LLVMNaClTransforms 
* LLVMNaClAnalysis 
* LLVMNaClBitReader
* LLVMCppBackendCodeGen 
* LLVMCppBackendInfo
* LLVMMCJIT 
* LLVMJIT 
* LLVMRuntimeDyld 
* LLVMExecutionEngine 
* LLVMCodeGen 
* LLVMObjCARCOpts 
* LLVMScalarOpts
* LLVMInstCombine 
* LLVMTransformUtils 
* LLVMipa 
* LLVMAnalysis 
* LLVMTarget
* LLVMObject 
* LLVMCore 
* LLVMSupport 
* LLVMX86AsmParser 
* LLVMX86Disassembler
* LLVMMC 
* LLVMMCDisassembler 
* LLVMMCParser 
* LLVMCodeGen
* rt 
* zdl 
* m 
* bfd 
* iberty

## TODO

1. Add multisource support
2. Simplify driver usage
3. Errors reporting