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
<<<<<<< HEAD
#include <direct.h>
#define OS_NAME_STR "Windows"
#define setenv(k, v, o) _putenv_s(k, v)
#define getcwd _getcwd
=======
#define OS_NAME_STR "Windows"
#define setenv(k, v, o) _putenv_s(k, v)
>>>>>>> fix-ci-build
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
<<<<<<< HEAD
// std.native.sys Implementation
=======
// std.sys Implementation
>>>>>>> fix-ci-build
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

<<<<<<< HEAD
// set_env(key, val)
static Value sys_set_env(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return NIL_VAL;
    
    const char* key = AS_CSTRING(args[0]);
    const char* val = AS_CSTRING(args[1]);
    
    setenv(key, val, 1);
    return NIL_VAL;
}

// cwd() -> String
static Value sys_cwd(int argCount, Value* args) {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        return OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
    }
    return NIL_VAL;
}

// args() -> List<String> 
static Value sys_args(int argCount, Value* args) {
    if (vm.cliArgs) return OBJ_VAL(vm.cliArgs);
    return NIL_VAL;
}

// exec(command) -> int
static Value sys_exec(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NUMBER_VAL(-1);
    const char* command = AS_CSTRING(args[0]);
    int result = system(command);
    return NUMBER_VAL((double)result);
}

ObjModule* create_std_sys_module() {
    ObjString* name = copyString("std.native.sys", 14);
=======
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
>>>>>>> fix-ci-build
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "exit", sys_exit);
    defineModuleFn(module, "env", sys_env);
<<<<<<< HEAD
    defineModuleFn(module, "set_env", sys_set_env);
    defineModuleFn(module, "cwd", sys_cwd);
    defineModuleFn(module, "args", sys_args);
    defineModuleFn(module, "exec", sys_exec);
=======
    defineModuleFn(module, "args", sys_args);
>>>>>>> fix-ci-build
    
    defineModuleConst(module, "OS_NAME", OBJ_VAL(copyString(OS_NAME_STR, strlen(OS_NAME_STR))));
    defineModuleConst(module, "ARCH", OBJ_VAL(copyString(ARCH_STR, strlen(ARCH_STR))));

    pop(&vm);
    pop(&vm);
    return module;
}
