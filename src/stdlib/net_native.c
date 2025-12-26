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

// http_get(url) -> String
static Value native_http_get(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    // Stub
    printf("[WARN] std.native.net.http_get is a stub.\n");
    return OBJ_VAL(copyString("HTTP/1.1 200 OK\n\nHello from Stub!", 34));
}

// http_post(url, body) -> String
static Value native_http_post(int argCount, Value* args) {
    if (argCount < 2) return NIL_VAL;
    // Stub
    printf("[WARN] std.native.net.http_post is a stub.\n");
    return OBJ_VAL(copyString("HTTP/1.1 200 OK\n\nPosted!", 24));
}

ObjModule* create_std_net_module() {
    ObjString* name = copyString("std.native.net", 14);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "http_get", native_http_get);
    defineModuleFn(module, "http_post", native_http_post);

    pop(&vm);
    pop(&vm);
    return module;
}
