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

// Classification Macros
#define IS_NUMBER(v) (((v) & QNAN) != QNAN)
#define IS_NULL(v)   ((v) == (QNAN | TAG_NULL))
#define IS_BOOL(v)   (((v) | 1) == (QNAN | TAG_TRUE))
#define IS_OBJ(v)    (((v) & (SIGN_BIT | QNAN)) == (SIGN_BIT | QNAN))

// Compatibility aliases
#define IS_NIL(v)    IS_NULL(v)

// Conversion Macros
#define AS_NUMBER(v) valueToNum(v)
#define AS_BOOL(v)   ((v) == (QNAN | TAG_TRUE))
#define AS_OBJ(v)    ((Obj*)(uintptr_t)((v) & ~(SIGN_BIT | QNAN)))

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
#define NIL_VAL         NULL_VAL
#define BOOL_VAL(b)     ((b) ? (QNAN | TAG_TRUE) : (QNAN | TAG_FALSE))
#define OBJ_VAL(obj)    (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

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

