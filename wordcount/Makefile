include ./Makefile.env
CFLAGS+=-std=c99 -g


CXXFLAGS+=-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

LDLIBS=-ljit -Wl,--whole-archive -lmapreduce -lnetworking -Wl,--no-whole-archive \
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
-lLLVMMC \
-lrt -lzdl -lm -lbfd -liberty
LDFLAGS+=$(LDLIBS)

all: clean createdirs map.nexe reduce.nexe
	./genmanifest.sh

map.nexe: obj/map.o 
	@$(CC) -o map.nexe obj/map.o $(LDFLAGS)

reduce.nexe: obj/reduce.o
	@$(CC) -o reduce.nexe obj/reduce.o $(LDFLAGS)

obj/map.o: src/map.c
	@$(CC) -c -o obj/map.o ${CFLAGS} src/map.c

obj/reduce.o: src/reduce.c
	@$(CC) -c -o obj/reduce.o ${CFLAGS} src/reduce.c

createdirs:
	chmod u+rwx genmanifest.sh mr_start.sh
	@mkdir -p obj log

clean: 
	@rm -fr obj/
	@rm -fr log/
	@rm -f *.txt
	@rm -f manifest/*.manifest	
	@rm -f nvram/*.nvram
	@rm -f *.nexe
	@rm -f nameservice.log
	@rm -f data/*.res


