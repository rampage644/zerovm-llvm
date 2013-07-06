#include "dump_listener.h"

#include "llvm/ExecutionEngine/ObjectImage.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFunction.h"

#include "zvm.h"

CustomJITEventListener::~CustomJITEventListener() {

}

void CustomJITEventListener::NotifyFunctionEmitted(const llvm::Function &fn,
                                   void *addr, size_t size,
                                   const llvm::JITEventListener::EmittedFunctionDetails &efd) {
  llvm::errs() << "function emitted:" << efd.MF->getName() << " addr=" << \
  addr << " size=" << size << "\n";
  
  if (int err = zvm_jail(addr, size) != 0) {
    llvm::errs() << "zvm_jail res: " << err << " errno="<< errno << "\n";
  }
}
void CustomJITEventListener::NotifyFreeingMachineCode(void *addr) {
  llvm::errs() << "function freed" << addr << "\n";
}
void CustomJITEventListener::NotifyObjectEmitted(const llvm::ObjectImage &Obj) {
  llvm::errs() << "obj emitted" << (uint8_t*)Obj.getData().data() << " " << Obj.getData().size() << "\n";
}
void CustomJITEventListener::NotifyFreeingObject(const llvm::ObjectImage &Obj) {
  llvm::errs() << "obj freed" << "\n";
}
