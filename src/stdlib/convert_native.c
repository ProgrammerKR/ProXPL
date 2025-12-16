// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Standard Library - Type Conversion Module
 * Native C implementation of type conversion functions
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/value.h"
#include "../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// to_int(value) - Convert to integer
static Value native_to_int(int argCount, Value* args) {
    if (argCount < 1) return MAKE_NUMBER(0);
    
    if (IS_NUMBER(args[0])) {
        return MAKE_NUMBER((double)(int)AS_NUMBER(args[0]));
    } else if (IS_STRING(args[0])) {
        const char* str = AS_CSTRING(args[0]);
        char* endptr;
        long value = strtol(str, &endptr, 10);
        return MAKE_NUMBER((double)value);
    } else if (IS_BOOL(args[0])) {
        return MAKE_NUMBER(AS_BOOL(args[0]) ? 1.0 : 0.0);
    }
    
    return MAKE_NUMBER(0);
}

// to_float(value) - Convert to float
static Value native_to_float(int argCount, Value* args) {
    if (argCount < 1) return MAKE_NUMBER(0.0);
    
    if (IS_NUMBER(args[0])) {
        return args[0];
    } else if (IS_STRING(args[0])) {
        const char* str = AS_CSTRING(args[0]);
        double value = strtod(str, NULL);
        return MAKE_NUMBER(value);
    } else if (IS_BOOL(args[0])) {
        return MAKE_NUMBER(AS_BOOL(args[0]) ? 1.0 : 0.0);
    }
    
    return MAKE_NUMBER(0.0);
}

// to_string(value) - Convert to string
static Value native_to_string(int argCount, Value* args) {
    if (argCount < 1) return MAKE_OBJ(copyString("", 0));
    
    char buffer[256];
    
    if (IS_NUMBER(args[0])) {
        snprintf(buffer, sizeof(buffer), "%.15g", AS_NUMBER(args[0]));
    } else if (IS_BOOL(args[0])) {
        snprintf(buffer, sizeof(buffer), "%s", AS_BOOL(args[0]) ? "true" : "false");
    } else if (IS_NULL(args[0])) {
        snprintf(buffer, sizeof(buffer), "null");
    } else if (IS_STRING(args[0])) {
        return args[0];
    } else {
        snprintf(buffer, sizeof(buffer), "<object>");
    }
    
    return MAKE_OBJ(copyString(buffer, strlen(buffer)));
}

// to_bool(value) - Convert to boolean
static Value native_to_bool(int argCount, Value* args) {
    if (argCount < 1) return MAKE_BOOL(false);
    
    if (IS_BOOL(args[0])) {
        return args[0];
    } else if (IS_NULL(args[0])) {
        return MAKE_BOOL(false);
    } else if (IS_NUMBER(args[0])) {
        return MAKE_BOOL(AS_NUMBER(args[0]) != 0);
    } else if (IS_STRING(args[0])) {
        return MAKE_BOOL(AS_STRING(args[0])->length > 0);
    }
    
    return MAKE_BOOL(true);
}

// to_hex(value) - Convert integer to hexadecimal string
static Value native_to_hex(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) {
        return MAKE_OBJ(copyString("0x0", 3));
    }
    
    int value = (int)AS_NUMBER(args[0]);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "0x%x", value);
    
    return MAKE_OBJ(copyString(buffer, strlen(buffer)));
}

// to_bin(value) - Convert integer to binary string
static Value native_to_bin(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) {
        return MAKE_OBJ(copyString("0b0", 3));
    }
    
    int value = (int)AS_NUMBER(args[0]);
    char buffer[35] = "0b";
    int pos = 2;
    
    if (value == 0) {
        buffer[pos++] = '0';
    } else {
        int bits[32];
        int count = 0;
        unsigned int uval = (unsigned int)value;
        
        while (uval > 0) {
            bits[count++] = uval % 2;
            uval /= 2;
        }
        
        for (int i = count - 1; i >= 0; i--) {
            buffer[pos++] = '0' + bits[i];
        }
    }
    
    buffer[pos] = '\0';
    return MAKE_OBJ(copyString(buffer, pos));
}

// char_at(str, index) - Get character at index
static Value native_char_at(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1])) {
        return MAKE_NULL();
    }
    
    ObjString* str = AS_STRING(args[0]);
    int index = (int)AS_NUMBER(args[1]);
    
    if (index < 0 || index >= (int)str->length) {
        return MAKE_NULL();
    }
    
    char ch[2] = {str->chars[index], '\0'};
    return MAKE_OBJ(copyString(ch, 1));
}

// len(value) - Get length of string or collection
static Value native_len(int argCount, Value* args) {
    if (argCount < 1) return MAKE_NUMBER(0);
    
    if (IS_STRING(args[0])) {
        return MAKE_NUMBER((double)AS_STRING(args[0])->length);
    }
    // TODO: Add support for lists, dicts, etc.
    
    return MAKE_NUMBER(0);
}

// Register all conversion functions with the VM
void register_convert_natives(VM* vm) {
    defineNative(vm, "to_int", native_to_int);
    defineNative(vm, "to_float", native_to_float);
    defineNative(vm, "to_string", native_to_string);
    defineNative(vm, "to_bool", native_to_bool);
    defineNative(vm, "to_hex", native_to_hex);
    defineNative(vm, "to_bin", native_to_bin);
    defineNative(vm, "char_at", native_char_at);
    defineNative(vm, "len", native_len);
}
