FLAG=d
NAME=jit

ARCH=x86_64-nacl

CC=${ARCH}-gcc
AR=${ARCH}-ar
CXX=${ARCH}-g++
RANLIB=${ARCH}-ranlib

CXXFLAGS=-Wall -Wno-long-long -msse4.1 -m64

ZVM_PREFIX_ABSPATH = $(abspath $(ZVM_PREFIX))
ZEROVM=${ZVM_PREFIX_ABSPATH}/bin/zerovm

# switch realease /debug mode
# possible values are: r for release, d for debug


LDLIBS=-ljit \
-lclangFrontendTool -lclangFrontend -lclangSerialization -lclangDriver -lclangCodeGen -lclangParse \
-lclangSema -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers \
-lclangStaticAnalyzerCore -lclangAnalysis -lclangRewriteFrontend -lclangRewriteCore \
-lclangARCMigrate \
-lclangEdit -lclangAST -lclangLex -lclangBasic -lLLVMInstrumentation \
-lLLVMAsmParser -lLLVMLinker -lLLVMipo -lLLVMVectorize -lLLVMBitWriter -lLLVMBitReader \
-lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils \
-lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMMCParser \
-lLLVMMCJIT -lLLVMJIT -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMScalarOpts \
-lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget \
-lLLVMObject -lLLVMCore -lLLVMSupport \
-lLLVMMC -lrt -lzdl -lm -lbfd -liberty

# add -g if debugging
ifeq ($(FLAG), d)
  CFLAGS+=-g
else
  CFLAGS+=-DNDEBUG
endif

OBJECTS := main.o

all: $(NAME).nexe
	python tests.py

$(NAME).nexe: $(OBJECTS)
	@echo "Compiling $(NAME).nexe"
	$(CC) -o $(NAME).nexe $(OBJECTS) $(CFLAGS) $(LDLIBS)

clean:
	@rm $(NAME).nexe 2>/dev/null || true
	@rm *.o *.conf *.manifest stdout* stderr* *.log 2>/dev/null || true


	

