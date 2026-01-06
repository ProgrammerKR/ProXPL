#ifndef PROX_BACKEND_LLVM_H
#define PROX_BACKEND_LLVM_H

#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

void emitLLVM(IRModule* module);

#ifdef __cplusplus
}
#endif

#endif // PROX_BACKEND_LLVM_H
