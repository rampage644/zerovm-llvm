#ifndef DUMP_LISTENER_H
#define DUMP_LISTENER_H

#include "llvm/ExecutionEngine/JITEventListener.h"

namespace llvm {
  class Function;
  class ObjectImage;
}

// first try class to listen to jit events and dump info
class CustomJITEventListener : public llvm::JITEventListener {
public:
  virtual ~CustomJITEventListener();

  virtual void NotifyFunctionEmitted(const llvm::Function &fn,
                                     void *addr, size_t size,
                                     const llvm::JITEventListener::EmittedFunctionDetails &efd);
  virtual void NotifyFreeingMachineCode(void *addr);
  virtual void NotifyObjectEmitted(const llvm::ObjectImage &Obj);
  virtual void NotifyFreeingObject(const llvm::ObjectImage &Obj);
};

#endif // DUMP_LISTENER_H