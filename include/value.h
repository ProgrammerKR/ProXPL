// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_VALUE_H
#define PROX_VALUE_H

#include "common.h"
#include <string.h>

// NaN-Boxing definitions
// Value moved to common.h

#define SIGN_BIT ((uint64_t)0x8000000000000000)
#define QNAN     ((uint64_t)0x7ff8000000000000)

#define TAG_NULL  1
#define TAG_FALSE 2
#define TAG_TRUE  3

// Classification Macros
#define IS_NUMBER(v) (((v) & QNAN) != QNAN)
#define IS_NULL(v)   ((v) == (QNAN | TAG_NULL))
#define IS_BOOL(v)   (((v) | 1) == (QNAN | TAG_TRUE))
#define IS_OBJ(v)    (((v) & (SIGN_BIT | QNAN)) == (SIGN_BIT | QNAN))
#define IS_NIL(v)    IS_NULL(v)

// Conversion Macros
#define AS_NUMBER(v) valueToNum(v)
#define AS_BOOL(v)   ((v) == (QNAN | TAG_TRUE))
#define AS_OBJ(v)    ((Obj*)(uintptr_t)((v) & ~(SIGN_BIT | QNAN)))

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

struct ValueArray {
  int capacity;
  int count;
  Value *values;
};

#ifdef __cplusplus
extern "C" {
#endif

void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value);

#ifdef __cplusplus
}
#endif

#endif // PROX_VALUE_H
