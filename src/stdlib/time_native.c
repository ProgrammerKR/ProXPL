// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

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

// now() -> Number (timestamp in seconds)
static Value native_now(int argCount, Value* args) {
    (void)argCount; (void)args;
    return NUMBER_VAL((double)time(NULL));
}

// clock() -> Number (CPU time in seconds)
Value native_clock(int argCount, Value* args) {
    (void)argCount; (void)args;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

// sleep(ms)
static Value native_sleep(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NIL_VAL;
    
    int ms = (int)AS_NUMBER(args[0]);
    
#ifdef _WIN32
    Sleep(ms); // Windows Sleep is in milliseconds
#else
    usleep(ms * 1000); // usleep is in microseconds
#endif

    return NIL_VAL;
}

// strftime(format, timestamp) -> String
static Value native_strftime(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) return NIL_VAL;
    
    const char* format = AS_CSTRING(args[0]);
    time_t timestamp = (time_t)AS_NUMBER(args[1]);
    
    struct tm* tm_info = localtime(&timestamp);
    char buffer[128];
    
    if (strftime(buffer, sizeof(buffer), format, tm_info)) {
        return OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
    }
    return NIL_VAL;
}

// timestamp(year, month, day, hour, min, sec) -> Number
// Year: e.g. 2025
// Month: 1-12
// Day: 1-31
static Value native_timestamp(int argCount, Value* args) {
    struct tm tm_info = {0};
    
    if (argCount > 0 && IS_NUMBER(args[0])) tm_info.tm_year = (int)AS_NUMBER(args[0]) - 1900;
    if (argCount > 1 && IS_NUMBER(args[1])) tm_info.tm_mon = (int)AS_NUMBER(args[1]) - 1;
    if (argCount > 2 && IS_NUMBER(args[2])) tm_info.tm_mday = (int)AS_NUMBER(args[2]);
    if (argCount > 3 && IS_NUMBER(args[3])) tm_info.tm_hour = (int)AS_NUMBER(args[3]);
    if (argCount > 4 && IS_NUMBER(args[4])) tm_info.tm_min = (int)AS_NUMBER(args[4]);
    if (argCount > 5 && IS_NUMBER(args[5])) tm_info.tm_sec = (int)AS_NUMBER(args[5]);
    
    time_t raw_time = mktime(&tm_info);
    if (raw_time == -1) return NIL_VAL;
    
    return NUMBER_VAL((double)raw_time);
}

ObjModule* create_std_time_module() {
    ObjString* name = copyString("std.native.time", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "now", native_now);
    defineModuleFn(module, "clock", native_clock);
    defineModuleFn(module, "sleep", native_sleep);
    
    // New functions
    defineModuleFn(module, "strftime", native_strftime);
    defineModuleFn(module, "timestamp", native_timestamp);

    pop(&vm);
    pop(&vm);
    return module;
}
