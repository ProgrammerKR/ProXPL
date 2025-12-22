// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define OS_NAME_STR "Windows"
#define setenv(k, v, o) _putenv_s(k, v)
#else
#include <unistd.h>
#define OS_NAME_STR "Linux/Unix"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_STR "x64"
#elif defined(__i386) || defined(_M_IX86)
#define ARCH_STR "x86"
#elif defined(__aarch64__)
#define ARCH_STR "arm64"
#else
#define ARCH_STR "unknown"
#endif

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/memory.h"

extern VM vm;

// Helper to define native function/constants in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

static void defineModuleConst(ObjModule* module, const char* name, Value value) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, value);
    tableSet(&module->exports, nameObj, value);
    pop(&vm);
    pop(&vm);
}

// --------------------------------------------------
// std.sys Implementation
// --------------------------------------------------

// exit(code)
static Value sys_exit(int argCount, Value* args) {
    int code = 0;
    if (argCount > 0 && IS_NUMBER(args[0])) {
        code = (int)AS_NUMBER(args[0]);
    }
    exit(code);
    return NIL_VAL; // Unreachable
}

// env(key) -> String or Null
static Value sys_env(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* key = AS_CSTRING(args[0]);
    char* val = getenv(key);
    
    if (val) {
        return OBJ_VAL(copyString(val, (int)strlen(val)));
    }
    return NIL_VAL;
}

// args() -> List<String> 
// Note: We don't have List object in C yet easily, so let's return Array or just count for now
// Phase 1 limitation: accessing global args requires VM support to store them first (in main.c)
// For now, return empty list or simulated args
static Value sys_args(int argCount, Value* args) {
    // TODO: Implement proper args passing from main.c -> VM
    return NIL_VAL;
}

ObjModule* create_std_sys_module() {
    ObjString* name = copyString("std.sys", 7);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "exit", sys_exit);
    defineModuleFn(module, "env", sys_env);
    defineModuleFn(module, "args", sys_args);
    
    defineModuleConst(module, "OS_NAME", OBJ_VAL(copyString(OS_NAME_STR, strlen(OS_NAME_STR))));
    defineModuleConst(module, "ARCH", OBJ_VAL(copyString(ARCH_STR, strlen(ARCH_STR))));

    pop(&vm);
    pop(&vm);
    return module;
}
