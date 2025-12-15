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

// --- Macro Compatibility Layer (CRITICAL FIX) ---
// This ensures that your MAKE_ macros return the correct 'Value' struct
// and not an integer, preventing Windows build errors.

#ifndef NIL_VAL
 #define NIL_VAL ((Value){VAL_NIL, { .number = 0 }})
#endif

#ifndef BOOL_VAL
 #define BOOL_VAL(value) ((Value){VAL_BOOL, { .boolean = value }})
#endif

#undef MAKE_NULL
#define MAKE_NULL() NIL_VAL

#undef MAKE_BOOL
#define MAKE_BOOL(x) BOOL_VAL(x)

#undef MAKE_OBJ
#define MAKE_OBJ(x) ((Value){VAL_OBJ, { .obj = (Obj*)(x) }})
// ------------------------------------------------

// print(...) - Print values to stdout
static Value native_print(int argCount, Value* args) {
    for (int i = 0; i < argCount; i++) {
        printValue(args[i]);
        if (i < argCount - 1) printf(" ");
    }
    printf("\n");
    return MAKE_NULL();
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
        return MAKE_OBJ(copyString(buffer, (int)len));
    }
    
    return MAKE_NULL();
}

// read_file(path) - Read entire file as string
static Value native_read_file(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        fprintf(stderr, "read_file expects a string path\n");
        return MAKE_NULL();
    }
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "rb");
    if (!file) {
        // Return NULL if file not found instead of crashing/printing
        return MAKE_NULL();
    }
    
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    
    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return MAKE_NULL();
    }
    
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    
    Value result = MAKE_OBJ(copyString(buffer, (int)bytesRead));
    free(buffer);
    return result;
}

// write_file(path, content) - Write string to file
static Value native_write_file(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0])) {
        fprintf(stderr, "write_file expects (path, content)\n");
        return MAKE_BOOL(false);
    }
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "w");
    if (!file) {
        return MAKE_BOOL(false);
    }

    if (IS_STRING(args[1])) {
        // Fast path: Write raw string directly
        ObjString* strObj = AS_STRING(args[1]);
        fwrite(strObj->chars, 1, strObj->length, file);
    } else {
        // Fallback: simple implementation for non-strings
        // Note: For full support, you'd want a valueToString helper here
        fprintf(file, "<non-string value>");
    }
    
    fclose(file);
    return MAKE_BOOL(true);
}

// append_file(path, content) - Append string to file
static Value native_append_file(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0])) {
        fprintf(stderr, "append_file expects (path, content)\n");
        return MAKE_BOOL(false);
    }
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "a");
    if (!file) {
        return MAKE_BOOL(false);
    }
    
    if (IS_STRING(args[1])) {
        ObjString* strObj = AS_STRING(args[1]);
        fwrite(strObj->chars, 1, strObj->length, file);
    } else {
        fprintf(file, "<non-string value>");
    }
    
    fclose(file);
    return MAKE_BOOL(true);
}

// Register all I/O functions with the VM
void register_io_natives(VM* vm) {
    defineNative(vm, "print", native_print);
    defineNative(vm, "input", native_input);
    defineNative(vm, "read_file", native_read_file);
    defineNative(vm, "write_file", native_write_file);
    defineNative(vm, "append_file", native_append_file);
}

