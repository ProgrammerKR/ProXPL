// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2026-04-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - Process Module
 * Environment variables, current directory, process-level info.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <psapi.h>
  #include <direct.h>
  #define getcwd _getcwd
#else
  #include <unistd.h>
#endif

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

extern VM vm;

static void defineModuleFn(ObjModule* module, const char* name, NativeFn fn) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(fn)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// process.env_get(name) -> string | nil
static Value native_env_get(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* val = getenv(AS_CSTRING(args[0]));
    if (!val) return NIL_VAL;
    return OBJ_VAL(copyString(val, (int)strlen(val)));
}

// process.env_set(name, value) -> bool
static Value native_env_set(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
#ifdef _WIN32
    // SetEnvironmentVariable only affects current process on Windows
    BOOL ok = SetEnvironmentVariableA(AS_CSTRING(args[0]), AS_CSTRING(args[1]));
    return BOOL_VAL(ok != 0);
#else
    int rc = setenv(AS_CSTRING(args[0]), AS_CSTRING(args[1]), 1);
    return BOOL_VAL(rc == 0);
#endif
}

// process.cwd() -> string
static Value native_cwd(int argCount, Value* args) {
    char buf[4096];
    if (!getcwd(buf, sizeof(buf))) return NIL_VAL;
    return OBJ_VAL(copyString(buf, (int)strlen(buf)));
}

// process.pid() -> number
static Value native_pid(int argCount, Value* args) {
#ifdef _WIN32
    return NUMBER_VAL((double)GetCurrentProcessId());
#else
    return NUMBER_VAL((double)getpid());
#endif
}

// process.cpu_count() -> number  (logical cores)
static Value native_cpu_count(int argCount, Value* args) {
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return NUMBER_VAL((double)si.dwNumberOfProcessors);
#else
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    return NUMBER_VAL(n > 0 ? (double)n : 1.0);
#endif
}

// process.memory_usage() -> number  (bytes, current process RSS on Windows)
static Value native_memory_usage(int argCount, Value* args) {
#ifdef _WIN32
    // Use PROCESS_MEMORY_COUNTERS
    // Avoid linking Psapi by using GetProcessMemoryInfo via LoadLibrary
    // Simple fallback: return 0 if unavailable
    typedef BOOL (WINAPI *PGPMI)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
    HMODULE hPsapi = LoadLibraryA("psapi.dll");
    if (hPsapi) {
        PGPMI fn = (PGPMI)GetProcAddress(hPsapi, "GetProcessMemoryInfo");
        if (fn) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (fn(GetCurrentProcess(), &pmc, sizeof(pmc))) {
                FreeLibrary(hPsapi);
                return NUMBER_VAL((double)pmc.WorkingSetSize);
            }
        }
        FreeLibrary(hPsapi);
    }
#endif
    return NUMBER_VAL(0);
}

// process.exit(code) -> never
static Value native_process_exit(int argCount, Value* args) {
    int code = (argCount >= 1 && IS_NUMBER(args[0])) ? (int)AS_NUMBER(args[0]) : 0;
    exit(code);
    return NIL_VAL; // unreachable
}

ObjModule* create_std_process_module() {
    ObjString* name = copyString("std.native.process", 18);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));

    defineModuleFn(module, "env_get",      native_env_get);
    defineModuleFn(module, "env_set",      native_env_set);
    defineModuleFn(module, "cwd",          native_cwd);
    defineModuleFn(module, "pid",          native_pid);
    defineModuleFn(module, "cpu_count",    native_cpu_count);
    defineModuleFn(module, "memory_usage", native_memory_usage);
    defineModuleFn(module, "exit",         native_process_exit);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}
