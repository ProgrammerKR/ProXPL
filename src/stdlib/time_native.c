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
    return NUMBER_VAL((double)time(NULL));
}

// clock() -> Number (CPU time in seconds)
static Value native_clock(int argCount, Value* args) {
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

ObjModule* create_std_time_module() {
    ObjString* name = copyString("std.native.time", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "now", native_now);
    defineModuleFn(module, "clock", native_clock);
    defineModuleFn(module, "sleep", native_sleep);

    pop(&vm);
    pop(&vm);
    return module;
}
