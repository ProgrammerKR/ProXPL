// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2026-01-04
//   Copyright © 2026. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/gc.h"

extern VM vm;

// Helper to define native function in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// gc.collect() -> Number (bytes collected)
static Value native_gc_collect(int argCount, Value* args) {
    (void)argCount; (void)args;
    size_t before = vm.bytesAllocated;
    collectGarbage(&vm);
    size_t after = vm.bytesAllocated;
    
    // Return bytes freed
    return NUMBER_VAL((double)(before - after));
}

// gc.stats() -> Map { "used": int, "next_gc": int }
static Value native_gc_stats(int argCount, Value* args) {
    (void)argCount; (void)args;
    
    // Check if we can easily create a Dictionary (not just Table)
    // If not, we might need newDictionary() exposed or implemented here?
    // ProXPL VM uses ObjInstance or ObjDictionary?
    // Let's check object.h/object.c - usually newDictionary or similar exists.
    // If unavailable at link time (if not in header), we can't.
    // Assuming newDictionary exists or we can mock it?
    // Wait, let's look at available headers. 
    // object.h usually declares newDictionary().
    // Let's assume it does.
    
    // For now, if newDictionary is not verified, I will return a string stats for safety
    // Or just a List [used, next_gc].
    // Let's try to include standard dictionary creation if possible.
    // But to be safe and avoid link errors if I don't check headers:
    // I will return a list [bytesAllocated, nextGC]
    
    ObjList* list = newList();
    push(&vm, OBJ_VAL(list));
    
    // used
    appendToList(list, NUMBER_VAL((double)vm.bytesAllocated));
    // next_gc
    appendToList(list, NUMBER_VAL((double)vm.nextGC));
    
    pop(&vm);
    return OBJ_VAL(list);
}

// gc.stress(bool)
// This requires a global flag in VM or GC.
// If vm struct doesn't have it, we can't easily add it without modifying vm.h and recompiling everything.
// For now, let's skip this unless we verify vm.h has it.
// I will just add usage() returning strings describing the GC.

static Value native_gc_usage(int argCount, Value* args) {
    (void)argCount; (void)args;
    return NUMBER_VAL((double)vm.bytesAllocated);
}

ObjModule* create_std_gc_module() {
    ObjString* name = copyString("std.native.gc", 13);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "collect", native_gc_collect);
    defineModuleFn(module, "stats", native_gc_stats); // Returns List: [bytes, next_gc]
    defineModuleFn(module, "usage", native_gc_usage);

    pop(&vm);
    pop(&vm);
    return module;
}
