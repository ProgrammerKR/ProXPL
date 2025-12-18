// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_VALUE_H
#define PROX_VALUE_H

#include "common.h"
#include <string.h>

// NaN-Boxing:
// IEEE 754 doubles have 52 bits of mantissa.
// specific NaN patterns can encode pointers (48 bits on x64) and tags.
//
// Mask: 0xFFFF000000000000 (Top 16 bits are tag/sign/exp)
//
// QNaN   : 0x7ff8000000000000
// SIGN   : 0x8000000000000000 (Sign bit implies object pointer)
// TAGS   : Lower 3 bits of the high 16 bits can be types.

typedef uint64_t Value;

// Masks
#define SIGN_BIT ((uint64_t)0x8000000000000000)
#define QNAN     ((uint64_t)0x7ffc000000000000)

#define TAG_NULL  1 // 01
#define TAG_FALSE 2 // 10
#define TAG_TRUE  3 // 11

typedef struct Obj Obj;

<<<<<<< HEAD
typedef enum { 
  VAL_BOOL, 
  VAL_NULL, // Kept for backward compatibility
  VAL_NUMBER, 
  VAL_OBJ 
} ValueType;

// --- COMPATIBILITY FIX: Map NIL to NULL ---
// This fixes the "VAL_NIL undeclared identifier" errors
#define VAL_NIL VAL_NULL
// ------------------------------------------
=======
// Classification Macros
#define IS_NUMBER(v) (((v) & QNAN) != QNAN)
#define IS_NULL(v)   ((v) == (QNAN | TAG_NULL))
#define IS_BOOL(v)   (((v) | 1) == (QNAN | TAG_TRUE))
#define IS_OBJ(v)    (((v) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))
>>>>>>> feature/opcode-tests

// Conversion Macros
#define AS_NUMBER(v) valueToNum(v)
#define AS_BOOL(v)   ((v) == (QNAN | TAG_TRUE))
#define AS_OBJ(v)    ((Obj*)(uintptr_t)((v) & ~(SIGN_BIT | QNAN)))

<<<<<<< HEAD
// Macros for checking type
#define IS_BOOL(value)   ((value).type == VAL_BOOL)
#define IS_NULL(value)   ((value).type == VAL_NULL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value)    ((value).type == VAL_OBJ)

// --- COMPATIBILITY FIX: Map IS_NIL to IS_NULL ---
#define IS_NIL(value)    IS_NULL(value)
// ------------------------------------------------

// Macros for unwrapping
#define AS_BOOL(value)   ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value)    ((value).as.obj)

// Macros for creating values
#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NULL_VAL          ((Value){VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((Value){VAL_OBJ, {.obj = (Obj *)object}})

// --- COMPATIBILITY FIX: Map NIL_VAL to NULL_VAL ---
#define NIL_VAL           NULL_VAL
// --------------------------------------------------
=======
// Value Constructors
static inline Value numToValue(double num) {
    Value value;
    memcpy(&value, &num, sizeof(double));
    return value;
}

static inline double valueToNum(Value value) {
    double num;
    memcpy(&num, &value, sizeof(Value));
    return num;
}

#define NUMBER_VAL(num) (numToValue(num))
#define NULL_VAL        ((Value)(QNAN | TAG_NULL))
#define BOOL_VAL(b)     ((b) ? (QNAN | TAG_TRUE) : (QNAN | TAG_FALSE))
#define OBJ_VAL(obj)    (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))
>>>>>>> feature/opcode-tests

// Value Array (for constants, stack etc.)
typedef struct {
  int capacity;
  int count;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value);

#endif

