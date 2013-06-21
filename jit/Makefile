# switch realease /debug mode
# possible values are: r for release, d for debug
FLAG=r
ifeq ($(FLAG),d)
	BUILD_DIR=build
	OUTPUT_DIR=Release
else
	BUILD_DIR=build_debug
	OUTPUT_DIR=Debug+Asserts
endif

LLVM_DIR=/home/ramp/git/llvm
LLVM_BUILD_DIR=/home/ramp/git/llvm/$(BUILD_DIR)
LDLIBS=-L$(LLVM_BUILD_DIR)/$(OUTPUT_DIR)/lib \
-lclangFrontend -lclangSerialization -lclangDriver -lclangCodeGen -lclangParse \
-lclangSema -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers \
-lclangStaticAnalyzerCore -lclangAnalysis -lclangRewriteCore -lclangRewriteFrontend \
-lclangEdit -lclangAST -lclangLex -lclangBasic -lLLVMIRReader -lLLVMInstrumentation \
-lLLVMAsmParser -lLLVMLinker -lLLVMipo -lLLVMVectorize -lLLVMBitWriter -lLLVMBitReader \
-lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils \
-lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMMCParser -lLLVMInterpreter -lLLVMJIT \
-lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMScalarOpts \
-lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget \
-lLLVMObject -lLLVMCore -lLLVMSupport -lLLVMX86AsmParser -lLLVMX86Disassembler \
-lLLVMMC -lLLVMMCDisassembler -lLLVMMCParser -lLLVMCodeGen \
-lrt -ldl -lm

NAME=jit
CC=x86_64-nacl-gcc
CFLAGS=-fno-rtti -DNDEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
# add -g if debugging
ifeq ($(FLAG), d)
	CFLAGS+=-g
endif

manifest:
	python run.py

$(NAME):
	@$(CC) -o $(NAME).nexe jit.cpp dump_listener.cpp \
	-I$(LLVM_DIR)/include -I$(LLVM_BUILD_DIR)/include \
	-I$(LLVM_BUILD_DIR)/tools/clang/include -I$(LLVM_DIR)/tools/clang/include \
	$(CFLAGS) $(LDLIBS)
run:
	zerovm -PQ $(NAME).manifest
	@cat stdout.jit
	@echo "-------------------------"
	@cat stderr.jit


clean:
	@rm $(NAME).nexe 2&>/dev/null || true
	@rm *.conf *.manifest stdout* stderr* *.log 2&>/dev/null || true
	

