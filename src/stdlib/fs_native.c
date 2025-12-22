// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/memory.h"

// Access VM
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

// --------------------------------------------------
// std.fs Implementation
// --------------------------------------------------

// readFile(path) -> String or Null
static Value fs_readFile(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "rb");
    if (!file) return NIL_VAL;
    
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    
    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return NIL_VAL;
    }
    
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    
    Value result = OBJ_VAL(takeString(buffer, (int)bytesRead));
    return result;
}

// writeFile(path, content) -> Bool
static Value fs_writeFile(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "w");
    if (!file) return BOOL_VAL(false);
    
    ObjString* content = AS_STRING(args[1]);
    fwrite(content->chars, 1, content->length, file);
    fclose(file);
    return BOOL_VAL(true);
}

// exists(path) -> Bool
static Value fs_exists(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    
    FILE* file = fopen(AS_CSTRING(args[0]), "r");
    if (file) {
        fclose(file);
        return BOOL_VAL(true);
    }
    return BOOL_VAL(false);
}

ObjModule* create_std_fs_module() {
    ObjString* name = copyString("std.fs", 6);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "readFile", fs_readFile);
    defineModuleFn(module, "writeFile", fs_writeFile);
    defineModuleFn(module, "exists", fs_exists);
    
    pop(&vm);
    pop(&vm);
    return module;
}
