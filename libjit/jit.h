#ifndef LIBJIT_JIT_H__
#define LIBJIT_JIT_H__

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Get pointer to function defined in some file
///
/// \p filename file function defined in
/// \p function function name
/// \returns function pointer on success, NULL otherwise
void* getFunctionPointer(const char* filename,
                         const char* function);

/// \brief Cleaning up after jitting
///
/// Should be called after all function pointers no longer needed
/// Otherwise client app could crash or memory leak
void shutdown();

/// \brief Returns last error description
///
/// Only usabel after getFunctionPointer() call. Should be used to get
/// human-readable error description after getFunctionPointer() fail.
///
/// \returns error description
const char* getLastError();

#ifdef __cplusplus
}
#endif

#endif // LIBJIT_JIT_H__
