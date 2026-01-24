// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - I/O Module (Native)
 * Native C implementation of low-level I/O functions
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

// print_raw(str) - Print string to stdout without newline
static Value native_print_raw(int argCount, Value* args) {
    if (argCount > 0) {
        if (IS_STRING(args[0])) {
            printf("%s", AS_CSTRING(args[0]));
        } else {
            printValue(args[0]);
        }
    }
    return NIL_VAL;
}

// eprint_raw(str) - Print string to stderr
static Value native_eprint_raw(int argCount, Value* args) {
    if (argCount > 0) {
        if (IS_STRING(args[0])) {
            fprintf(stderr, "%s", AS_CSTRING(args[0]));
        } else {
            // printValue prints to stdout usually, so manual handling might be needed
            // For now, just print what we can or rely on printValue if it was flexible
            // But let's assume we pass strings primarily
             fprintf(stderr, "<value>"); 
        }
    }
    return NIL_VAL;
}

// println(val) - Print value with newline
static Value native_println(int argCount, Value* args) {
    if (argCount > 0) {
        if (IS_STRING(args[0])) {
            printf("%s\n", AS_CSTRING(args[0]));
        } else {
            printValue(args[0]);
            printf("\n");
        }
    } else {
        printf("\n");
    }
    return NIL_VAL;
}

// input_raw() - Read line from stdin
static Value native_input_raw(int argCount, Value* args) {
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove newline(s)
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[len-1] = '\0';
            len--;
        }
        return OBJ_VAL(copyString(buffer, (int)len));
    }
    return NIL_VAL;
}

// input(prompt) - Display prompt and read line from stdin
static Value native_input(int argCount, Value* args) {
    // Print optional prompt
    if (argCount > 0 && IS_STRING(args[0])) {
        printf("%s", AS_CSTRING(args[0]));
        fflush(stdout);
    }
    
    // Read input
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove newline(s)
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[len-1] = '\0';
            len--;
        }
        return OBJ_VAL(copyString(buffer, (int)len));
    }
    return NIL_VAL;
}

// flush_raw() - Flush stdout
static Value native_flush_raw(int argCount, Value* args) {
    fflush(stdout);
    return NIL_VAL;
}

// set_color_raw(code) - Set terminal color using ANSI
static Value native_set_color_raw(int argCount, Value* args) {
    if (argCount > 0 && IS_NUMBER(args[0])) {
        int code = (int)AS_NUMBER(args[0]);
        printf("\033[%dm", code);
    }
    return NIL_VAL;
}

// Register all I/O functions with the VM
ObjModule* create_std_io_module() {
    ObjString* name = copyString("std.native.io", 13);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "print_raw", native_print_raw);
    defineModuleFn(module, "print", native_print_raw);
    defineModuleFn(module, "write", native_print_raw);
    defineModuleFn(module, "println", native_println);
    defineModuleFn(module, "eprint_raw", native_eprint_raw);
    defineModuleFn(module, "input_raw", native_input_raw);
    defineModuleFn(module, "input", native_input);
    defineModuleFn(module, "flush_raw", native_flush_raw);
    defineModuleFn(module, "set_color_raw", native_set_color_raw);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}

// Register I/O functions as globals (for convenience)
void register_io_globals(VM* vm) {
    defineNative(vm, "print", native_print_raw);
    defineNative(vm, "println", native_println);
    defineNative(vm, "input", native_input);
}
