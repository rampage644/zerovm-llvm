#ifndef LIBJIT_JIT_H__
#define LIBJIT_JIT_H__

extern "C" {

/// \brief Get pointer to function defined in some file
///
/// \p filename file function defined in
/// \p function function name
/// \returns function pointer on success, NULL otherwise
void* getFunctionPointer(const char* filename,
                         const char* function);

}

#endif // LIBJIT_JIT_H__
