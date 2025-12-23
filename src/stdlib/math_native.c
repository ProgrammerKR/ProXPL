// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Standard Library - Math Module  
 * Native C implementation of mathematical functions
 */

// CRITICAL FIX: This must be defined BEFORE <math.h> for Windows to see M_E, M_PI
#define _USE_MATH_DEFINES 

#include "common.h"
#include "vm.h"
#include "value.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// abs(x) - Absolute value
static Value native_abs(int argCount, Value* args) {
    if (argCount < 1) return NUMBER_VAL(0);
    
    if (IS_NUMBER(args[0])) {
        return NUMBER_VAL(fabs(AS_NUMBER(args[0])));
    }
    return args[0];
}

// ceil(x) - Ceiling function  
static Value native_ceil(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(ceil(AS_NUMBER(args[0])));
}

// floor(x) - Floor function
static Value native_floor(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(floor(AS_NUMBER(args[0])));
}

// round(x, decimals) - Round to n decimal places
static Value native_round(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    
    double value = AS_NUMBER(args[0]);
    int decimals = 0;
    
    if (argCount >= 2 && IS_NUMBER(args[1])) {
        decimals = (int)AS_NUMBER(args[1]);
    }
    
    double multiplier = pow(10.0, decimals);
    return NUMBER_VAL(round(value * multiplier) / multiplier);
}

// max(...) - Maximum value
static Value native_max(int argCount, Value* args) {
    if (argCount == 0) return NIL_VAL;
    
    double maxVal = IS_NUMBER(args[0]) ? AS_NUMBER(args[0]) : 0;
    for (int i = 1; i < argCount; i++) {
        if (IS_NUMBER(args[i])) {
            double val = AS_NUMBER(args[i]);
            if (val > maxVal) maxVal = val;
        }
    }
    return NUMBER_VAL(maxVal);
}

// min(...) - Minimum value
static Value native_min(int argCount, Value* args) {
    if (argCount == 0) return NIL_VAL;
    
    double minVal = IS_NUMBER(args[0]) ? AS_NUMBER(args[0]) : 0;
    for (int i = 1; i < argCount; i++) {
        if (IS_NUMBER(args[i])) {
            double val = AS_NUMBER(args[i]);
            if (val < minVal) minVal = val;
        }
    }
    return NUMBER_VAL(minVal);
}

// pow(base, exp) - Power function
static Value native_pow(int argCount, Value* args) {
    if (argCount < 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) {
        return NUMBER_VAL(0);
    }
    return NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
}

// sqrt(x) - Square root
static Value native_sqrt(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
}

// Trigonometric functions
static Value native_sin(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(sin(AS_NUMBER(args[0])));
}

static Value native_cos(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(cos(AS_NUMBER(args[0])));
}

static Value native_tan(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(tan(AS_NUMBER(args[0])));
}

// log(x, base) - Logarithm
static Value native_log(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    
    double x = AS_NUMBER(args[0]);
    double base = M_E;
    
    if (argCount >= 2 && IS_NUMBER(args[1])) {
        base = AS_NUMBER(args[1]);
    }
    
    if (base == M_E) {
        return NUMBER_VAL(log(x));
    } else {
        return NUMBER_VAL(log(x) / log(base));
    }
}

// exp(x) - Exponential function
static Value native_exp(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(exp(AS_NUMBER(args[0])));
}

// random() - Random number [0, 1)
static Value native_random(int argCount, Value* args) {
    return NUMBER_VAL((double)rand() / RAND_MAX);
}

// randint(min, max) - Random integer in [min, max]
static Value native_randint(int argCount, Value* args) {
    if (argCount < 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) {
        return NUMBER_VAL(0);
    }
    
    int min = (int)AS_NUMBER(args[0]);
    int max = (int)AS_NUMBER(args[1]);
    
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    
    int range = max - min + 1;
    return NUMBER_VAL(min + (rand() % range));
}

// Register all math functions with the VM
void register_math_natives(VM* vm) {
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    defineNative(vm, "abs", native_abs);
    defineNative(vm, "ceil", native_ceil);
    defineNative(vm, "floor", native_floor);
    defineNative(vm, "round", native_round);
    defineNative(vm, "max", native_max);
    defineNative(vm, "min", native_min);
    defineNative(vm, "pow", native_pow);
    defineNative(vm, "sqrt", native_sqrt);
    defineNative(vm, "sin", native_sin);
    defineNative(vm, "cos", native_cos);
    defineNative(vm, "tan", native_tan);
    defineNative(vm, "log", native_log);
    defineNative(vm, "exp", native_exp);
    defineNative(vm, "random", native_random);
    defineNative(vm, "randint", native_randint);
}

