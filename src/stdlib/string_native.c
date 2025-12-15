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

// --- Safety Macros ---
// These ensure we use the correct return types (Value struct) 
// instead of accidentally returning ints.
#ifndef NIL_VAL
  #define NIL_VAL ((Value){VAL_NIL, { .number = 0 }})
#endif

// str_upper(str) - Convert to uppercase
static Value native_str_upper(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    ObjString* original = AS_STRING(args[0]);
    char* buffer = (char*)malloc(original->length + 1);
    
    for (int i = 0; i < original->length; i++) {
        // Cast to unsigned char to safely handle non-ASCII characters
        buffer[i] = toupper((unsigned char)original->chars[i]);
    }
    buffer[original->length] = '\0';
    
    Value result = OBJ_VAL(copyString(buffer, original->length));
    free(buffer);
    return result;
}

// str_lower(str) - Convert to lowercase
static Value native_str_lower(int argCount, Value* args) {
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

// str_trim(str) - Remove leading/trailing whitespace
static Value native_str_trim(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* str = AS_CSTRING(args[0]);
    int len = AS_STRING(args[0])->length;
    
    // Find start
    int start = 0;
    while (start < len && isspace((unsigned char)str[start])) start++;
    
    // Find end
    int end = len - 1;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    
    int newLen = end - start + 1;
    if (newLen <= 0) return OBJ_VAL(copyString("", 0));
    
    return OBJ_VAL(copyString(str + start, newLen));
}

// str_split(str, delimiter) - Split string by delimiter
static Value native_str_split(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return NIL_VAL;
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
    const char* new_str = AS_CSTRING(args[2]); // Renamed 'new' to 'new_str' (C++ keyword safety)
    
    int oldLen = AS_STRING(args[1])->length;
    int newLen = AS_STRING(args[2])->length;
    
    if (oldLen == 0) return args[0];
    
    // Simple implementation: replace first occurrence
    const char* pos = strstr(str, old);
    if (!pos) return args[0];
    
    int prefixLen = (int)(pos - str);
    int suffixLen = (int)strlen(pos + oldLen);
    int totalLen = prefixLen + newLen + suffixLen;
    
    char* buffer = (char*)malloc(totalLen + 1);
    
    // Copy prefix
    memcpy(buffer, str, prefixLen);
    // Copy replacement
    memcpy(buffer + prefixLen, new_str, newLen);
    // Copy suffix
    memcpy(buffer + prefixLen + newLen, pos + oldLen, suffixLen);
    
    buffer[totalLen] = '\0';
    
    Value result = OBJ_VAL(copyString(buffer, totalLen));
    free(buffer);
    return result;
}

// str_contains(str, substr) - Check if string contains substring
static Value native_str_contains(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* substr = AS_CSTRING(args[1]);
    
    return BOOL_VAL(strstr(str, substr) != NULL);
}

// str_startswith(str, prefix) - Check if string starts with prefix
static Value native_str_startswith(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }
    
    const char* str = AS_CSTRING(args[0]);
    const char* prefix = AS_CSTRING(args[1]);
    int prefixLen = AS_STRING(args[1])->length;
    
    return BOOL_VAL(strncmp(str, prefix, prefixLen) == 0);
}

// str_endswith(str, suffix) - Check if string ends with suffix
static Value native_str_endswith(int argCount, Value* args) {
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

// substr(str, start, length) - Extract substring
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
    
    // Ensure length is not negative
    if (length < 0) length = 0;
    
    return OBJ_VAL(copyString(str + start, length));
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

