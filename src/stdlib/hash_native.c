// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25
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

// Helper
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// Simple hash (DJB2) for now unless we link OpenSSL/etc.
static unsigned long djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

// md5(str) -> String (Stub/Simple Hash)
static Value native_md5(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    // Real MD5 requires implementation. Returning a placeholder hash for MVP.
    unsigned long h = djb2(AS_CSTRING(args[0]));
    char buffer[32];
    sprintf(buffer, "%lx", h);
    return OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
}

// sha256(str) -> String (Stub)
static Value native_sha256(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    // Real SHA256 requires implementation.
    return native_md5(argCount, args);
}

ObjModule* create_std_hash_module() {
    ObjString* name = copyString("std.native.hash", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "md5", native_md5);
    defineModuleFn(module, "sha256", native_sha256);

    pop(&vm);
    pop(&vm);
    return module;
}
