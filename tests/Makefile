NAME=MCJITTests
FLAG=d

include ../Makefile.env

LDLIBS = -L$(LLVM_BUILD_DIR)/$(OUTPUT_DIR)/lib -L$(CLANG_BUILD_DIR)/$(OUTPUT_DIR)/lib \
-lLLVMMCJIT -lLLVMJIT -lLLVMRuntimeDyld -lLLVMObject \
-lLLVMExecutionEngine -lLLVMCore -lLLVMSupport -lrt -ldl -lm
OBJECTS = 

${NAME}.nexe: $(OBJECTS)
	@ln -s $(LLVM_BUILD_DIR)/unittests/ExecutionEngine/MCJIT/$(OUTPUT_DIR)/$(NAME) $(NAME).nexe