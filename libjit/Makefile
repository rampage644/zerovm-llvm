FLAG=r
NAME=libjit

ARCH=x86_64-nacl

CC=${ARCH}-gcc
AR=${ARCH}-ar
CXX=${ARCH}-g++
RANLIB=${ARCH}-ranlib

CXXFLAGS=-Wall -Wno-long-long -msse4.1 -m64 -fno-exceptions -fno-rtti

FLAG?=r
ifeq ($(FLAG),d)
	BUILD_DIR=build_debug
	OUTPUT_DIR=Debug+Asserts

	LLVM_DIR=/home/zvm/git/llvm-nacl
	LLVM_BUILD_DIR=$(LLVM_DIR)/$(BUILD_DIR)
	CLANG_BUILD_DIR=$(LLVM_DIR)/tools/clang
	CLANG_DIR=$(LLVM_DIR)/tools/clang

	LDLIBS=-L${LLVM_DIR} -L${LLVM_BUILD_DIR}/${OUTPUT}/lib -L${CLANG_DIR} -L${CLANG_BUILD_DIR}/${OUTPUT}/lib
endif

ifeq ($(FLAG), d)
  CXXFLAGS+=-g
else
  CXXFLAGS+=-DNDEBUG
endif



CXXFLAGS+=-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

LDLIBS+=-Wl,--whole-archive -lmapreduce -lnetworking -Wl,--no-whole-archive \
-lclangFrontendTool -lclangFrontend -lclangSerialization -lclangDriver -lclangCodeGen -lclangParse \
-lclangAnalysis \
-lclangEdit -lclangAST -lclangLex -lclangBasic -lLLVMInstrumentation \
-lLLVMAsmParser -lLLVMLinker -lLLVMipo -lLLVMVectorize -lLLVMBitWriter -lLLVMBitReader \
-lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils \
-lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMMCParser \
-lLLVMMCJIT -lLLVMJIT -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMScalarOpts \
-lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget \
-lLLVMObject -lLLVMCore -lLLVMSupport \
-lLLVMMC \
-lrt -lzdl -lm -lbfd -liberty -lgtest

ZVM_PREFIX_ABSPATH = $(abspath $(ZVM_PREFIX))
ZEROVM=${ZVM_PREFIX_ABSPATH}/bin/zerovm

OBJECTS := jit.o

all: tests

jit.o: jit.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(NAME).a: $(OBJECTS)
	@echo "Compiling $(NAME).a"
	$(AR) rcs $(NAME).a $(OBJECTS) 

tests.o: tests.cpp
	$(CXX) -c -o tests.o tests.cpp $(CXXFLAGS)	-I.
tests.nexe: tests.o $(NAME).a
	$(CXX) -o $@ $< libjit.a $(LDLIBS) -L. 
tests: tests.nexe $(NAME).a
	@python run.py
	@zerovm -PQst tests.manifest || true
	@cat stdout.tests
	@cat stderr.tests

debug: tests
	@zgdb tests.nexe

clean:
	@rm *.o *.nexe *.a *.conf *.log std* *.manifest

install:
	install -D -m 0755 $(NAME).a $(ZVM_PREFIX_ABSPATH)/x86_64-nacl/lib
	install -D -m 0644 jit.h $(ZVM_PREFIX_ABSPATH)/x86_64-nacl/include
	

