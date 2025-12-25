#ifndef PROX_GC_H
#define PROX_GC_H

#include "common.h"
#include "value.h"
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize GC state
void initGC(VM* vm);

// Free all objects (called at VM shutdown)
void freeObjects(VM* vm);

// Trigger a garbage collection cycle
void collectGarbage(VM* vm);

// Mark a generic object as reachable
void markObject(Obj* object);

// Mark a value as reachable
void markValue(Value value);

// Allocation wrapper that triggers GC if needed
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#ifdef __cplusplus
}
#endif

#endif // PROX_GC_H
