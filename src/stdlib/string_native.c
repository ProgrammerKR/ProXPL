// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - String Module
 * Native C implementation of string manipulation functions
 */

#include "common.h"
#include "vm.h"
#include "value.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// upper(str) - Convert to uppercase
static Value native_upper(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    ObjString* original = AS_STRING(args[0]);
    char* buffer = (char*)malloc(original->length + 1);
    
    for (int i = 0; i < original->length; i++) {
        buffer[i] = toupper((unsigned char)original->chars[i]);
    }
    buffer[original->length] = '\0';
    
    Value result = OBJ_VAL(copyString(buffer, original->length));
    free(buffer);
    return result;
}

// lower(str) - Convert to lowercase
static Value native_lower(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    ObjString* original = AS_STRING(args[0]);
    char* buffer = (char*)malloc(original->length + 1);
    
    for (int i = 0; i < original->length; i++) {
        buffer[i] = tolower((unsigned char)original->chars[i]);
    }
    buffer[original->length] = '\0';
    
    Value result = OBJ_VAL(copyString(buffer, original->length));
    free(buffer);
    return result;
}

// trim(str) - Remove leading/trailing whitespace
static Value native_trim(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* str = AS_CSTRING(args[0]);
    int len = AS_STRING(args[0])->length;
    
    int start = 0;
    while (start < len && isspace((unsigned char)str[start])) start++;
    
    int end = len - 1;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    
    int newLen = end - start + 1;
    if (newLen <= 0) return OBJ_VAL(copyString("", 0));
    
    return OBJ_VAL(copyString(str + start, newLen));
}

// split(str, delimiter) - Split string by delimiter
static Value native_split(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return NIL_VAL;
    }
    
    // TODO: Return array/list once collections are implemented
    return args[0];
}

// replace(str, old, new) - Replace occurrences
static Value native_replace(int argCount, Value* args) {
    if (argCount < 3 || !IS_STRING(args[0]) || 
        !IS_STRING(args[1]) || !IS_STRING(args[2])) {
        return args[0];
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* old = AS_CSTRING(args[1]);
    const char* new_str = AS_CSTRING(args[2]); 
    
    int oldLen = AS_STRING(args[1])->length;
    int newLen = AS_STRING(args[2])->length;
    
    if (oldLen == 0) return args[0];
    
    const char* pos = strstr(str, old);
    if (!pos) return args[0];
    
    int prefixLen = (int)(pos - str);
    int suffixLen = (int)strlen(pos + oldLen);
    int totalLen = prefixLen + newLen + suffixLen;
    
    char* buffer = (char*)malloc(totalLen + 1);
    
    memcpy(buffer, str, prefixLen);
    memcpy(buffer + prefixLen, new_str, newLen);
    memcpy(buffer + prefixLen + newLen, pos + oldLen, suffixLen);
    
    buffer[totalLen] = '\0';
    
    Value result = OBJ_VAL(copyString(buffer, totalLen));
    free(buffer);
    return result;
}

// contains(str, substr)
static Value native_contains(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* substr = AS_CSTRING(args[1]);
    
    return BOOL_VAL(strstr(str, substr) != NULL);
}

// startswith(str, prefix)
static Value native_startswith(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* prefix = AS_CSTRING(args[1]);
    int prefixLen = AS_STRING(args[1])->length;
    
    return BOOL_VAL(strncmp(str, prefix, prefixLen) == 0);
}

// endswith(str, suffix)
static Value native_endswith(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }
    
    int strLen = AS_STRING(args[0])->length;
    int suffixLen = AS_STRING(args[1])->length;
    
    if (suffixLen > strLen) return BOOL_VAL(false);
    
    const char* str = AS_CSTRING(args[0]);
    const char* suffix = AS_CSTRING(args[1]);
    
    return BOOL_VAL(strcmp(str + strLen - suffixLen, suffix) == 0);
}

// substr(str, start, length)
static Value native_substr(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) {
        return NIL_VAL;
    }
    
    const char* str = AS_CSTRING(args[0]);
    int strLen = AS_STRING(args[0])->length;
    int start = (int)AS_NUMBER(args[1]);
    
    if (start < 0) start = 0;
    if (start >= strLen) return OBJ_VAL(copyString("", 0));
    
    int length = strLen - start;
    if (argCount >= 3 && IS_NUMBER(args[2])) {
        length = (int)AS_NUMBER(args[2]);
    }
    
    if (start + length > strLen) {
        length = strLen - start;
    }
    
    if (length < 0) length = 0;
    
    return OBJ_VAL(copyString(str + start, length));
}

ObjModule* create_std_str_module() {
    ObjString* name = copyString("std.native.str", 14);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "upper", native_upper);
    defineModuleFn(module, "lower", native_lower);
    defineModuleFn(module, "trim", native_trim);
    defineModuleFn(module, "split", native_split);
    defineModuleFn(module, "replace", native_replace);
    defineModuleFn(module, "contains", native_contains);
    defineModuleFn(module, "startswith", native_startswith);
    defineModuleFn(module, "endswith", native_endswith);
    defineModuleFn(module, "substr", native_substr);

    pop(&vm);
    pop(&vm);
    return module;
}

// Register string functions as globals
void register_string_globals(VM* vm) {
    defineNative(vm, "upper", native_upper);
    defineNative(vm, "lower", native_lower);
    defineNative(vm, "trim", native_trim);
    defineNative(vm, "split", native_split);
    defineNative(vm, "replace", native_replace);
    defineNative(vm, "contains", native_contains);
    defineNative(vm, "startswith", native_startswith);
    defineNative(vm, "endswith", native_endswith);
    // substr is already registered in stdlib_core.c, but no harm re-registering or skipping.
    // stdlib_core.c handles substr separately for some reason.
}
