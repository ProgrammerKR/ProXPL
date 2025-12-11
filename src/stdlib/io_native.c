/*
 * ProXPL Standard Library - I/O Module
 * Native C implementation of I/O functions
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/value.h"
#include "../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        }
        return MAKE_OBJ(copyString(buffer, strlen(buffer)));
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
        fprintf(stderr, "Could not open file: %s\n", path);
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
    
    Value result = MAKE_OBJ(copyString(buffer, bytesRead));
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
    
    // Convert content to string
    char buffer[4096];
    if (IS_STRING(args[1])) {
        snprintf(buffer, sizeof(buffer), "%s", AS_CSTRING(args[1]));
    } else {
        // TODO: Convert other types to string
        snprintf(buffer, sizeof(buffer), "<value>");
    }
    
    FILE* file = fopen(path, "w");
    if (!file) {
        return MAKE_BOOL(false);
    }
    
    fputs(buffer, file);
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
    
    char buffer[4096];
    if (IS_STRING(args[1])) {
        snprintf(buffer, sizeof(buffer), "%s", AS_CSTRING(args[1]));
    } else {
        snprintf(buffer, sizeof(buffer), "<value>");
    }
    
    FILE* file = fopen(path, "a");
    if (!file) {
        return MAKE_BOOL(false);
    }
    
    fputs(buffer, file);
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
