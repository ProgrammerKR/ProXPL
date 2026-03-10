// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#ifndef PROX_PXCF_H
#define PROX_PXCF_H

#include "../include/common.h"
#include "../include/value.h"
#include "../include/vm.h"

#ifdef __cplusplus
extern "C" {
#endif

// Parses a PXCF formatted string and returns an ObjDictionary Value
Value parsePxcf(VM* vm, const char* content);

// Native function signature for loadConfig
Value nativeLoadConfig(int argCount, Value *args);

#ifdef __cplusplus
}
#endif

#endif // PROX_PXCF_H
