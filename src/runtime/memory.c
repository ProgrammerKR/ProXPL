// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdlib.h>

#include "../include/memory.h"
#include "../include/vm.h"
#include "../include/gc.h"

extern VM vm;

// void* reallocate(void* pointer, size_t oldSize, size_t newSize)
// Moved to src/runtime/gc.c to handle GC triggers.

// void freeObjects()
// Moved to src/runtime/gc.c
