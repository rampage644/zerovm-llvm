FLAG=d
NAME=jit

include ../Makefile.env

# switch realease /debug mode
# possible values are: r for release, d for debug


LDLIBS=-L$(LLVM_BUILD_DIR)/$(OUTPUT_DIR)/lib -L$(CLANG_BUILD_DIR)/$(OUTPUT_DIR)/lib \
-lclangFrontendTool -lclangFrontend -lclangSerialization -lclangDriver -lclangCodeGen -lclangParse \
-lclangSema -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers \
-lclangStaticAnalyzerCore -lclangAnalysis -lclangRewriteFrontend -lclangRewriteCore \
-lclangARCMigrate \
-lclangEdit -lclangAST -lclangLex -lclangBasic -lLLVMIRReader -lLLVMInstrumentation \
-lLLVMAsmParser -lLLVMLinker -lLLVMipo -lLLVMVectorize -lLLVMBitWriter -lLLVMBitReader \
-lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils \
-lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMMCParser -lLLVMInterpreter -lLLVMJIT \
-lLLVMNaClBitWriter -lLLVMNaClTransforms -lLLVMNaClAnalysis -lLLVMNaClBitReader \
-lLLVMCppBackendCodeGen -lLLVMCppBackendInfo \
-lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMMCJIT -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMScalarOpts \
-lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget \
-lLLVMObject -lLLVMCore -lLLVMSupport -lLLVMX86AsmParser -lLLVMX86Disassembler \
-lLLVMMC -lLLVMMCDisassembler -lLLVMMCParser -lLLVMCodeGen \
-lrt -ldl -lm

CXXFLAGS+=-fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
# add -g if debugging
ifeq ($(FLAG), d)
  CXXFLAGS+=-g
else
  CXXFLAGS+=-DNDEBUG
endif

#OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
OBJECTS := jit.o dump_listener.o

$(NAME).nexe: $(OBJECTS)
	@echo "Compiling $(NAME).nexe"
	@$(CXX) -o $(NAME).nexe $(OBJECTS) $(CXXFLAGS) $(LDLIBS)

	

