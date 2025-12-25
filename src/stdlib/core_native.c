// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

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

// --------------------------------------------------
// std.core Implementation
// --------------------------------------------------

// assert(condition, message)
static Value core_assert(int argCount, Value* args) {
    if (argCount < 1) {
        // Runtime error is too complex to trigger from here without access to runtimeError
        // For now, just print and exit if false
        fprintf(stderr, "Assertion failed: No condition provided.\n");
        exit(1);
    }
    
    if (isFalsey(args[0])) {
        fprintf(stderr, "Assertion failed");
        if (argCount > 1 && IS_STRING(args[1])) {
            fprintf(stderr, ": %s", AS_CSTRING(args[1]));
        }
        fprintf(stderr, "\n");
        exit(1); // Hard crash for assertion failure
    }
    
    return NIL_VAL;
}

// typeOf(value) -> String
static Value core_typeOf(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;
    
    Value v = args[0];
    const char* typeStr = "unknown";
    
    if (IS_BOOL(v)) {
        typeStr = "bool";
    } else if (IS_NIL(v)) {
        typeStr = "nil";
    } else if (IS_NUMBER(v)) {
        typeStr = "number";
    } else if (IS_OBJ(v)) {
        switch (OBJ_TYPE(v)) {
            case OBJ_STRING: typeStr = "string"; break;
            case OBJ_FUNCTION: typeStr = "function"; break;
            case OBJ_NATIVE: typeStr = "native"; break;
            case OBJ_MODULE: typeStr = "module"; break;
            default: typeStr = "object"; break;
        }
    }
    
    return OBJ_VAL(copyString(typeStr, (int)strlen(typeStr)));
}

// unwrap(optional) -> Value or Panic
// This assumes we have Option types, which we don't really have yet. 
// For now, it basically ensuring value is not nil.
static Value core_unwrap(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;
    
    if (IS_NIL(args[0])) {
        fprintf(stderr, "Panic: Called unwrap() on nil value.\n");
        exit(1);
    }
    
    return args[0];
}

ObjModule* create_std_core_module() {
    ObjString* name = copyString("std.core", 8);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "assert", core_assert);
    defineModuleFn(module, "typeOf", core_typeOf);
    defineModuleFn(module, "unwrap", core_unwrap);
    
    pop(&vm);
    pop(&vm);
    return module;
}
