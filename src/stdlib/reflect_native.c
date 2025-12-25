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

// type_of(val) -> String
static Value native_type_of(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;
    Value v = args[0];
    
    if (IS_NULL(v)) return OBJ_VAL(copyString("null", 4));
    if (IS_BOOL(v)) return OBJ_VAL(copyString("bool", 4));
    if (IS_NUMBER(v)) return OBJ_VAL(copyString("number", 6));
    
    if (IS_OBJ(v)) {
        switch (OBJ_TYPE(v)) {
            case OBJ_STRING: return OBJ_VAL(copyString("string", 6));
            case OBJ_FUNCTION: return OBJ_VAL(copyString("function", 8));
            case OBJ_NATIVE: return OBJ_VAL(copyString("native", 6));
            case OBJ_MODULE: return OBJ_VAL(copyString("module", 6));
            case OBJ_CLASS: return OBJ_VAL(copyString("class", 5));
            case OBJ_INSTANCE: {
                ObjInstance* instance = AS_INSTANCE(v);
                return OBJ_VAL(copyString(instance->klass->name->chars, instance->klass->name->length));
            }
            case OBJ_BOUND_METHOD: return OBJ_VAL(copyString("method", 6));
            default: return OBJ_VAL(copyString("object", 6));
        }
    }
    
    return OBJ_VAL(copyString("unknown", 7));
}

ObjModule* create_std_reflect_module() {
    ObjString* name = copyString("std.native.reflect", 18);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "type_of", native_type_of);

    pop(&vm);
    pop(&vm);
    return module;
}
