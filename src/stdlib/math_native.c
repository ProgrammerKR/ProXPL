// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Standard Library - Math Module  
 * Native C implementation of mathematical functions
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/value.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// abs(x) - Absolute value
static Value native_abs(int argCount, Value* args) {
    if (argCount < 1) return MAKE_NUMBER(0);
    
    if (IS_NUMBER(args[0])) {
        return MAKE_NUMBER(fabs(AS_NUMBER(args[0])));
    }
    return args[0];
}

// ceil(x) - Ceiling function  
static Value native_ceil(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(ceil(AS_NUMBER(args[0])));
}

// floor(x) - Floor function
static Value native_floor(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(floor(AS_NUMBER(args[0])));
}

// round(x, decimals) - Round to n decimal places
static Value native_round(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    
    double value = AS_NUMBER(args[0]);
    int decimals = 0;
    
    if (argCount >= 2 && IS_NUMBER(args[1])) {
        decimals = (int)AS_NUMBER(args[1]);
    }
    
    double multiplier = pow(10.0, decimals);
    return MAKE_NUMBER(round(value * multiplier) / multiplier);
}

// max(...) - Maximum value
static Value native_max(int argCount, Value* args) {
    if (argCount == 0) return MAKE_NULL();
    
    double maxVal = IS_NUMBER(args[0]) ? AS_NUMBER(args[0]) : 0;
    for (int i = 1; i < argCount; i++) {
        if (IS_NUMBER(args[i])) {
            double val = AS_NUMBER(args[i]);
            if (val > maxVal) maxVal = val;
        }
    }
    return MAKE_NUMBER(maxVal);
}

// min(...) - Minimum value
static Value native_min(int argCount, Value* args) {
    if (argCount == 0) return MAKE_NULL();
    
    double minVal = IS_NUMBER(args[0]) ? AS_NUMBER(args[0]) : 0;
    for (int i = 1; i < argCount; i++) {
        if (IS_NUMBER(args[i])) {
            double val = AS_NUMBER(args[i]);
            if (val < minVal) minVal = val;
        }
    }
    return MAKE_NUMBER(minVal);
}

// pow(base, exp) - Power function
static Value native_pow(int argCount, Value* args) {
    if (argCount < 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) {
        return MAKE_NUMBER(0);
    }
    return MAKE_NUMBER(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
}

// sqrt(x) - Square root
static Value native_sqrt(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(sqrt(AS_NUMBER(args[0])));
}

// Trigonometric functions
static Value native_sin(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(sin(AS_NUMBER(args[0])));
}

static Value native_cos(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(cos(AS_NUMBER(args[0])));
}

static Value native_tan(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(tan(AS_NUMBER(args[0])));
}

// log(x, base) - Logarithm
static Value native_log(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    
    double x = AS_NUMBER(args[0]);
    double base = M_E;
    
    if (argCount >= 2 && IS_NUMBER(args[1])) {
        base = AS_NUMBER(args[1]);
    }
    
    if (base == M_E) {
        return MAKE_NUMBER(log(x));
    } else {
        return MAKE_NUMBER(log(x) / log(base));
    }
}

// exp(x) - Exponential function
static Value native_exp(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return MAKE_NUMBER(0);
    return MAKE_NUMBER(exp(AS_NUMBER(args[0])));
}

// random() - Random number [0, 1)
static Value native_random(int argCount, Value* args) {
    return MAKE_NUMBER((double)rand() / RAND_MAX);
}

// randint(min, max) - Random integer in [min, max]
static Value native_randint(int argCount, Value* args) {
    if (argCount < 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) {
        return MAKE_NUMBER(0);
    }
    
    int min = (int)AS_NUMBER(args[0]);
    int max = (int)AS_NUMBER(args[1]);
    
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    
    int range = max - min + 1;
    return MAKE_NUMBER(min + (rand() % range));
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
