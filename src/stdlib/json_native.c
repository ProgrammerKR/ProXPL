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

// Helper to define native function in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// parse(str) -> Object (Map/List)
// Limitation: Missing List/Map native creation API. 
// Returning dummy or Null for now until Object API is ready.
static Value native_json_parse(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    // TODO: Implement JSON parser
    printf("[WARN] std.native.json.parse is not fully implemented yet.\n");
    return NIL_VAL; 
}

// stringify(val) -> String
static Value native_json_stringify(int argCount, Value* args) {
    if (argCount < 1) return OBJ_VAL(copyString("", 0));
    
    // Basic implementation for primitives
    Value v = args[0];
    if (IS_NULL(v)) return OBJ_VAL(copyString("null", 4));
    if (IS_BOOL(v)) {
        return AS_BOOL(v) ? OBJ_VAL(copyString("true", 4)) : OBJ_VAL(copyString("false", 5));
    }
    if (IS_NUMBER(v)) {
        char buffer[32];
        snprintf(buffer, 32, "%.14g", AS_NUMBER(v));
        return OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
    }
    if (IS_STRING(v)) {
        // TODO: Escape string
        ObjString* s = AS_STRING(v);
        // Simple wrap in quotes for now
        int len = s->length + 2;
        char* buffer = (char*)malloc(len + 1);
        buffer[0] = '"';
        memcpy(buffer + 1, s->chars, s->length);
        buffer[len-1] = '"';
        buffer[len] = '\0';
        Value res = OBJ_VAL(takeString(buffer, len));
        return res;
    }

    return OBJ_VAL(copyString("[Object]", 8));
}

ObjModule* create_std_json_module() {
    ObjString* name = copyString("std.native.json", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "parse", native_json_parse);
    defineModuleFn(module, "stringify", native_json_stringify);

    pop(&vm);
    pop(&vm);
    return module;
}
