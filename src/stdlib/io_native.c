// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - I/O Module
 * Native C implementation of I/O functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "vm.h"
#include "value.h"
#include "object.h"
#include "memory.h"

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

// print(...) - Print values to stdout
static Value native_print(int argCount, Value* args) {
    for (int i = 0; i < argCount; i++) {
        printValue(args[i]);
        if (i < argCount - 1) printf(" ");
    }
    printf("\n");
    return NIL_VAL;
}

// input(prompt) - Read line from stdin
static Value native_input(int argCount, Value* args) {
    if (argCount > 0) {
        printValue(args[0]);
    }
    
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }
        return OBJ_VAL(copyString(buffer, (int)len));
    }
    
    return NIL_VAL;
}


// Register all I/O functions with the VM
ObjModule* create_std_io_module() {
    ObjString* name = copyString("std.io", 6);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "print", native_print);
    defineModuleFn(module, "input", native_input);
    
    pop(&vm); // module
    pop(&vm); // name
    return module;
}
