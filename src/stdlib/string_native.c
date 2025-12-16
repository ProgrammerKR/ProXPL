// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Standard Library - String Module
 * Native C implementation of string manipulation functions
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/value.h"
#include "../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// str_upper(str) - Convert to uppercase
static Value native_str_upper(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return MAKE_NULL();
    
    ObjString* original = AS_STRING(args[0]);
    char* buffer = (char*)malloc(original->length + 1);
    
    for (uint32_t i = 0; i < original->length; i++) {
        buffer[i] = toupper(original->chars[i]);
    }
    buffer[original->length] = '\0';
    
    Value result = MAKE_OBJ(copyString(buffer, original->length));
    free(buffer);
    return result;
}

// str_lower(str) - Convert to lowercase
static Value native_str_lower(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return MAKE_NULL();
    
    ObjString* original = AS_STRING(args[0]);
    char* buffer = (char*)malloc(original->length + 1);
    
    for (uint32_t i = 0; i < original->length; i++) {
        buffer[i] = tolower(original->chars[i]);
    }
    buffer[original->length] = '\0';
    
    Value result = MAKE_OBJ(copyString(buffer, original->length));
    free(buffer);
    return result;
}

// str_trim(str) - Remove leading/trailing whitespace
static Value native_str_trim(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return MAKE_NULL();
    
    const char* str = AS_CSTRING(args[0]);
    int len = AS_STRING(args[0])->length;
    
    // Find start
    int start = 0;
    while (start < len && isspace(str[start])) start++;
    
    // Find end
    int end = len - 1;
    while (end >= start && isspace(str[end])) end--;
    
    int newLen = end - start + 1;
    if (newLen <= 0) return MAKE_OBJ(copyString("", 0));
    
    return MAKE_OBJ(copyString(str + start, newLen));
}

// str_split(str, delimiter) - Split string by delimiter
static Value native_str_split(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return MAKE_NULL();
    }
    
    // TODO: Return array/list once collections are implemented
    // For now, return the original string
    return args[0];
}

// str_replace(str, old, new) - Replace occurrences
static Value native_str_replace(int argCount, Value* args) {
    if (argCount < 3 || !IS_STRING(args[0]) || 
        !IS_STRING(args[1]) || !IS_STRING(args[2])) {
        return args[0];
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* old = AS_CSTRING(args[1]);
    const char* new = AS_CSTRING(args[2]);
    
    int oldLen = AS_STRING(args[1])->length;
    int newLen = AS_STRING(args[2])->length;
    
    if (oldLen == 0) return args[0];
    
    // Simple implementation: replace first occurrence
    const char* pos = strstr(str, old);
    if (!pos) return args[0];
    
    int prefixLen = pos - str;
    int suffixLen = strlen(pos + oldLen);
    int totalLen = prefixLen + newLen + suffixLen;
    
    char* buffer = (char*)malloc(totalLen + 1);
    memcpy(buffer, str, prefixLen);
    memcpy(buffer + prefixLen, new, newLen);
    memcpy(buffer + prefixLen + newLen, pos + oldLen, suffixLen);
    buffer[totalLen] = '\0';
    
    Value result = MAKE_OBJ(copyString(buffer, totalLen));
    free(buffer);
    return result;
}

// str_contains(str, substr) - Check if string contains substring
static Value native_str_contains(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return MAKE_BOOL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* substr = AS_CSTRING(args[1]);
    
    return MAKE_BOOL(strstr(str, substr) != NULL);
}

// str_startswith(str, prefix) - Check if string starts with prefix
static Value native_str_startswith(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return MAKE_BOOL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* prefix = AS_CSTRING(args[1]);
    int prefixLen = AS_STRING(args[1])->length;
    
    return MAKE_BOOL(strncmp(str, prefix, prefixLen) == 0);
}

// str_endswith(str, suffix) - Check if string ends with suffix
static Value native_str_endswith(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return MAKE_BOOL(false);
    }
    
    int strLen = AS_STRING(args[0])->length;
    int suffixLen = AS_STRING(args[1])->length;
    
    if (suffixLen > strLen) return MAKE_BOOL(false);
    
    const char* str = AS_CSTRING(args[0]);
    const char* suffix = AS_CSTRING(args[1]);
    
    return MAKE_BOOL(strcmp(str + strLen - suffixLen, suffix) == 0);
}

// substr(str, start, length) - Extract substring
static Value native_substr(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) {
        return MAKE_NULL();
    }
    
    const char* str = AS_CSTRING(args[0]);
    int strLen = AS_STRING(args[0])->length;
    int start = (int)AS_NUMBER(args[1]);
    
    if (start < 0) start = 0;
    if (start >= strLen) return MAKE_OBJ(copyString("", 0));
    
    int length = strLen - start;
    if (argCount >= 3 && IS_NUMBER(args[2])) {
        length = (int)AS_NUMBER(args[2]);
    }
    
    if (start + length > strLen) {
        length = strLen - start;
    }
    
    return MAKE_OBJ(copyString(str + start, length));
}

// Register all string functions with the VM
void register_string_natives(VM* vm) {
    defineNative(vm, "str_upper", native_str_upper);
    defineNative(vm, "str_lower", native_str_lower);
    defineNative(vm, "str_trim", native_str_trim);
    defineNative(vm, "str_split", native_str_split);
    defineNative(vm, "str_replace", native_str_replace);
    defineNative(vm, "str_contains", native_str_contains);
    defineNative(vm, "str_startswith", native_str_startswith);
    defineNative(vm, "str_endswith", native_str_endswith);
    defineNative(vm, "substr", native_substr);
}
