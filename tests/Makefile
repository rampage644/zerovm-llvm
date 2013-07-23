NAME=test
FLAG=d

include ../Makefile.env

LDLIBS = -L$(LLVM_BUILD_DIR)/$(OUTPUT_DIR)/lib -L$(CLANG_BUILD_DIR)/$(OUTPUT_DIR)/lib \
-lLLVMMCJIT -lLLVMJIT -lLLVMRuntimeDyld -lLLVMObject \
-lLLVMExecutionEngine -lLLVMCore -lLLVMSupport -lrt -ldl -lm
OBJECTS = test.o

${NAME}.nexe: $(OBJECTS)
	@$(CXX) -o $(NAME).nexe $(OBJECTS) $(LDLIBS) -g