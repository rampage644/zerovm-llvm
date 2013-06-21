#include "dump_listener.h"

#include "llvm/ExecutionEngine/ObjectImage.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFunction.h"

CustomJITEventListener::~CustomJITEventListener() {

}

void CustomJITEventListener::NotifyFunctionEmitted(const llvm::Function &fn,
                                   void *addr, size_t size,
                                   const llvm::JITEventListener::EmittedFunctionDetails &efd) {
  llvm::errs() << "function emitted:" << efd.MF->getName() << addr << size;
  efd.MF->print(llvm::errs());
}
void CustomJITEventListener::NotifyFreeingMachineCode(void *addr) {
  llvm::errs() << "function freed" << addr;
}
void CustomJITEventListener::NotifyObjectEmitted(const llvm::ObjectImage &Obj) {
  llvm::errs() << "obj emitted";
}
void CustomJITEventListener::NotifyFreeingObject(const llvm::ObjectImage &Obj) {
  llvm::errs() << "obj freed";
}
