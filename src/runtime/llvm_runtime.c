// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../include/common.h"
#include "../include/value.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/vm.h" // For globals/strings table access if needed
#include <stdio.h>
#include <stdlib.h>

// Helper to wrap allocating a string from a constant C-string
Value prox_rt_const_string(const char* chars, int length) {
    // We copy it because ProXPL strings manage their own memory
    return OBJ_VAL(copyString(chars, length));
}

Value prox_rt_add(Value a, Value b) {
    if (IS_NUMBER(a) && IS_NUMBER(b)) {
        return NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b));
    }
    
    if (IS_STRING(a) && IS_STRING(b)) {
        ObjString* sA = AS_STRING(a);
        ObjString* sB = AS_STRING(b);
        
        int length = sA->length + sB->length;
        char* chars = ALLOCATE(char, length + 1);
        memcpy(chars, sA->chars, sA->length);
        memcpy(chars + sA->length, sB->chars, sB->length);
        chars[length] = '\0';
        
        return OBJ_VAL(takeString(chars, length));
    }
    
    // Runtime Error or return NULL/Error
    printf("Runtime Error: Invalid operands for +\n");
    return NIL_VAL;
}

void prox_rt_print(Value v) {
    printValue(v);
    printf("\n");
}
