// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define PLATFORM_NAME "windows"
#define POPEN _popen
#define PCLOSE _pclose
#else
#include <unistd.h>
#include <sys/utsname.h>
#define PLATFORM_NAME "linux" // or macos detect
#define POPEN popen
#define PCLOSE pclose
#endif

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

extern VM vm;

// Helper
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

// platform() -> String
static Value native_platform(int argCount, Value* args) {
    return OBJ_VAL(copyString(PLATFORM_NAME, (int)strlen(PLATFORM_NAME)));
}

// cpu_count() -> Number
static Value native_cpu_count(int argCount, Value* args) {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return NUMBER_VAL((double)sysinfo.dwNumberOfProcessors);
#else
    return NUMBER_VAL((double)sysconf(_SC_NPROCESSORS_ONLN));
#endif
}

// exec(cmd) -> String (Output)
static Value native_exec(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* cmd = AS_CSTRING(args[0]);
    FILE* pipe = POPEN(cmd, "r");
    if (!pipe) return NIL_VAL;
    
    char buffer[128];
    // Read output mostly to string. Limitation: large output.
    // For now, read first 1024 bytes or so? Or realloc.
    // Let's implement simple reader.
    
    // NOTE: Implementing full dynamic string builder here is tedious without helper.
    // Sticking to fixed buffer for MVP.
    char result[4096];
    result[0] = '\0';
    size_t currentLen = 0;
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        if (currentLen + len < sizeof(result)) {
            strcat(result, buffer);
            currentLen += len;
        } else {
            break; // Truncated
        }
    }
    
    PCLOSE(pipe);
    return OBJ_VAL(copyString(result, (int)currentLen));
}

ObjModule* create_std_os_module() {
    ObjString* name = copyString("std.native.os", 14);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "platform", native_platform);
    defineModuleFn(module, "cpu_count", native_cpu_count);
    defineModuleFn(module, "exec", native_exec);
    
    defineModuleConst(module, "PLATFORM", OBJ_VAL(copyString(PLATFORM_NAME, strlen(PLATFORM_NAME))));

    pop(&vm);
    pop(&vm);
    return module;
}
