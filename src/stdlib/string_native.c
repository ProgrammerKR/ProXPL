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
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// SIMD Includes
#if defined(_MSC_VER)
  // Windows/MSVC - Check for x86/x64 and AVX2 availability
  #if defined(_M_AMD64) || defined(_M_IX86)
    #include <intrin.h>
    #if defined(__AVX2__)
      #define PROX_SIMD_AVX2
    #endif
  #endif
#elif defined(__GNUC__) || defined(__clang__)
  // GCC/Clang - Check for x86/x64 and AVX2 availability
  #if defined(__x86_64__) || defined(__i386__)
    #include <immintrin.h>
    #ifdef __AVX2__
      #define PROX_SIMD_AVX2
    #endif
  #endif
#endif

extern VM vm;

// ----------------------------------------------------------------------------
// SIMD Accelerators
// ----------------------------------------------------------------------------

// AVX2 accelerated character search
// Returns index or -1 if not found
static int find_char_simd(const char* str, int len, char target) {
#ifdef PROX_SIMD_AVX2
    // Alignment checks skipped for brevity, loading unaligned
    
    __m256i vTarget = _mm256_set1_epi8(target); // Broadcast character
    int i = 0;
    
    // Process 32 bytes at a time
    for (; i <= len - 32; i += 32) {
        __m256i vChunk = _mm256_loadu_si256((const __m256i*)(str + i));
        __m256i vEq = _mm256_cmpeq_epi8(vChunk, vTarget);
        unsigned int mask = _mm256_movemask_epi8(vEq);
        
        if (mask != 0) {
            // Found in this chunk
            // Using built-in ctz (count trailing zeros) to find index
            #if defined(_MSC_VER)
                unsigned long idx;
                _BitScanForward(&idx, mask);
                return i + idx;
            #else
                return i + __builtin_ctz(mask);
            #endif
        }
    }
    
    // Scalar fallback for remaining
    for (; i < len; i++) {
        if (str[i] == target) return i;
    }
    return -1;
#else
    // Pure scalar
    const char* ptr = strchr(str, target);
    return ptr ? (int)(ptr - str) : -1;
#endif
}

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

// split(str, delimiter) - Split string by delimiter -> ObjList
static Value native_split(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return NIL_VAL;
    }

    ObjString* strObj = AS_STRING(args[0]);
    ObjString* delObj = AS_STRING(args[1]);
    const char* str   = strObj->chars;
    int  len   = strObj->length;
    const char* del   = delObj->chars;
    int  delLen= delObj->length;

    ObjList* list = newList();
    push(&vm, OBJ_VAL(list));

    if (delLen == 0) {
        // Split into individual characters
        for (int i = 0; i < len; i++) {
            Value ch = OBJ_VAL(copyString(str + i, 1));
            push(&vm, ch);
            if (list->capacity < list->count + 1) {
                int old = list->capacity;
                list->capacity = GROW_CAPACITY(old);
                list->items = GROW_ARRAY(Value, list->items, old, list->capacity);
            }
            list->items[list->count++] = pop(&vm);
        }
        return pop(&vm);
    }

    int pos = 0;
    while (pos <= len) {
        // For single-char delimiter use SIMD, otherwise strstr
        int next;
        if (delLen == 1) {
            int off = find_char_simd(str + pos, len - pos, del[0]);
            next = (off == -1) ? -1 : pos + off;
        } else {
            const char* found = strstr(str + pos, del);
            next = found ? (int)(found - str) : -1;
        }

        int tokenLen = (next == -1) ? (len - pos) : (next - pos);
        Value token = OBJ_VAL(copyString(str + pos, tokenLen));
        push(&vm, token);
        if (list->capacity < list->count + 1) {
            int old = list->capacity;
            list->capacity = GROW_CAPACITY(old);
            list->items = GROW_ARRAY(Value, list->items, old, list->capacity);
        }
        list->items[list->count++] = pop(&vm);

        if (next == -1) break;
        pos = next + delLen;
    }

    return pop(&vm);
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
    
    // Optim: If substr is 1 char, use SIMD
    if (AS_STRING(args[1])->length == 1) {
        int idx = find_char_simd(str, AS_STRING(args[0])->length, substr[0]);
        return BOOL_VAL(idx != -1);
    }
    
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

// repeat(str, n) - Repeat string n times
static Value native_repeat(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) return NIL_VAL;
    ObjString* s = AS_STRING(args[0]);
    int n = (int)AS_NUMBER(args[1]);
    if (n <= 0) return OBJ_VAL(copyString("", 0));
    int totalLen = s->length * n;
    char* buf = (char*)malloc(totalLen + 1);
    for (int i = 0; i < n; i++) memcpy(buf + i * s->length, s->chars, s->length);
    buf[totalLen] = '\0';
    Value result = OBJ_VAL(copyString(buf, totalLen));
    free(buf);
    return result;
}

// pad_left(str, width, padChar) - Left-pad string to width
static Value native_pad_left(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) return args[0];
    ObjString* s = AS_STRING(args[0]);
    int width = (int)AS_NUMBER(args[1]);
    char padCh = (argCount >= 3 && IS_STRING(args[2]) && AS_STRING(args[2])->length > 0)
                 ? AS_STRING(args[2])->chars[0] : ' ';
    if (s->length >= width) return args[0];
    int pad = width - s->length;
    char* buf = (char*)malloc(width + 1);
    memset(buf, padCh, pad);
    memcpy(buf + pad, s->chars, s->length);
    buf[width] = '\0';
    Value result = OBJ_VAL(copyString(buf, width));
    free(buf);
    return result;
}

// pad_right(str, width, padChar) - Right-pad string to width
static Value native_pad_right(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) return args[0];
    ObjString* s = AS_STRING(args[0]);
    int width = (int)AS_NUMBER(args[1]);
    char padCh = (argCount >= 3 && IS_STRING(args[2]) && AS_STRING(args[2])->length > 0)
                 ? AS_STRING(args[2])->chars[0] : ' ';
    if (s->length >= width) return args[0];
    int pad = width - s->length;
    char* buf = (char*)malloc(width + 1);
    memcpy(buf, s->chars, s->length);
    memset(buf + s->length, padCh, pad);
    buf[width] = '\0';
    Value result = OBJ_VAL(copyString(buf, width));
    free(buf);
    return result;
}

// count_occurrences(str, sub) -> number
static Value native_count_occurrences(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return NUMBER_VAL(0);
    const char* str = AS_CSTRING(args[0]);
    const char* sub = AS_CSTRING(args[1]);
    int subLen = AS_STRING(args[1])->length;
    if (subLen == 0) return NUMBER_VAL(0);
    int count = 0;
    const char* p = str;
    while ((p = strstr(p, sub)) != NULL) { count++; p += subLen; }
    return NUMBER_VAL((double)count);
}

// str_reverse(str) -> string
static Value native_str_reverse(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    ObjString* s = AS_STRING(args[0]);
    char* buf = (char*)malloc(s->length + 1);
    for (int i = 0; i < s->length; i++) buf[i] = s->chars[s->length - 1 - i];
    buf[s->length] = '\0';
    Value result = OBJ_VAL(copyString(buf, s->length));
    free(buf);
    return result;
}

// index_of(str, sub, from) -> number  (-1 if not found)
static Value native_index_of(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return NUMBER_VAL(-1);
    const char* str = AS_CSTRING(args[0]);
    const char* sub = AS_CSTRING(args[1]);
    int from = (argCount >= 3 && IS_NUMBER(args[2])) ? (int)AS_NUMBER(args[2]) : 0;
    int len  = AS_STRING(args[0])->length;
    if (from < 0) from = 0;
    if (from >= len) return NUMBER_VAL(-1);
    const char* found = strstr(str + from, sub);
    return found ? NUMBER_VAL((double)(found - str)) : NUMBER_VAL(-1);
}

// trim_left(str) / trim_right(str)
static Value native_trim_left(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* str = AS_CSTRING(args[0]);
    int len = AS_STRING(args[0])->length;
    int start = 0;
    while (start < len && isspace((unsigned char)str[start])) start++;
    return OBJ_VAL(copyString(str + start, len - start));
}

static Value native_trim_right(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* str = AS_CSTRING(args[0]);
    int len = AS_STRING(args[0])->length;
    int end = len - 1;
    while (end >= 0 && isspace((unsigned char)str[end])) end--;
    return OBJ_VAL(copyString(str, end + 1));
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
    defineModuleFn(module, "substr",             native_substr);
    defineModuleFn(module, "repeat",             native_repeat);
    defineModuleFn(module, "pad_left",           native_pad_left);
    defineModuleFn(module, "pad_right",          native_pad_right);
    defineModuleFn(module, "count_occurrences",  native_count_occurrences);
    defineModuleFn(module, "reverse",            native_str_reverse);
    defineModuleFn(module, "index_of",           native_index_of);
    defineModuleFn(module, "trim_left",          native_trim_left);
    defineModuleFn(module, "trim_right",         native_trim_right);

    pop(&vm);
    pop(&vm);
    return module;
}

// Register string functions as globals
void register_string_globals(VM* pVM) {
    defineNative(pVM, "upper",            native_upper);
    defineNative(pVM, "lower",            native_lower);
    defineNative(pVM, "trim",             native_trim);
    defineNative(pVM, "trim_left",        native_trim_left);
    defineNative(pVM, "trim_right",       native_trim_right);
    defineNative(pVM, "split",            native_split);
    defineNative(pVM, "replace",          native_replace);
    defineNative(pVM, "contains",         native_contains);
    defineNative(pVM, "startswith",       native_startswith);
    defineNative(pVM, "endswith",         native_endswith);
    defineNative(pVM, "repeat",           native_repeat);
    defineNative(pVM, "pad_left",         native_pad_left);
    defineNative(pVM, "pad_right",        native_pad_right);
    defineNative(pVM, "count_occurrences",native_count_occurrences);
    defineNative(pVM, "str_reverse",      native_str_reverse);
    defineNative(pVM, "index_of",         native_index_of);
}
