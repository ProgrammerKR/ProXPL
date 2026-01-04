#ifndef PROX_FFI_BRIDGE_H
#define PROX_FFI_BRIDGE_H

#include "value.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

// Call a foreign function interface object
Value callForeign(ObjForeign* foreign, int argCount, Value* args);

// Load a dynamic library and find a symbol
// Returns successful ObjForeign* or NULL if failed
// If libraryPath is NULL, looks in current process (e.g. libc/msvcrt)
ObjForeign* loadForeign(ObjString* libraryPath, ObjString* symbolName);

#ifdef __cplusplus
}
#endif

#endif // PROX_FFI_BRIDGE_H
