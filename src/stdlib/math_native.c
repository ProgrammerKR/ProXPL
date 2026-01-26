// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - Math Module  
 * Native C implementation of mathematical functions
 */

// CRITICAL FIX: This must be defined BEFORE <math.h> for Windows to see M_E, M_PI
#define _USE_MATH_DEFINES 

#include "common.h"
#include "vm.h"
#include "value.h"
#include "object.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern VM vm;

// Helper to define native function in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

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

static Value native_asin(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(asin(AS_NUMBER(args[0])));
}

static Value native_acos(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(acos(AS_NUMBER(args[0])));
}

static Value native_atan(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NUMBER_VAL(0);
    return NUMBER_VAL(atan(AS_NUMBER(args[0])));
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
    // If no arguments, return float [0.0, 1.0) - Standard behavior
    if (argCount == 0) {
        double random_part = (double)rand() / ((double)RAND_MAX + 1.0);
        return NUMBER_VAL(random_part);
    }

    // If arguments are provided, behave like randint (User request: "no decimals")
    // random(max) -> [0, max]
    // random(min, max) -> [min, max]
    
    double min = 0.0;
    double max = 1.0;

    if (argCount == 1) {
        if (!IS_NUMBER(args[0])) return NUMBER_VAL(0);
        max = AS_NUMBER(args[0]);
    } else if (argCount >= 2) {
        if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) return NUMBER_VAL(0);
        min = AS_NUMBER(args[0]);
        max = AS_NUMBER(args[1]);
    }

    if (min > max) {
        double temp = min;
        min = max;
        max = temp;
    }

    // Return integer
    long long min_int = (long long)min;
    long long max_int = (long long)max;
    long long range = max_int - min_int + 1;
    
    // Safety check for range 0 or negative (though swapped above)
    if (range <= 0) return NUMBER_VAL(min_int);

    return NUMBER_VAL((double)(min_int + (rand() % range)));
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

// seed(val)
static Value native_seed(int argCount, Value* args) {
    if (argCount > 0 && IS_NUMBER(args[0])) {
        srand((unsigned int)AS_NUMBER(args[0]));
    } else {
        srand((unsigned int)time(NULL));
    }
    return NIL_VAL;
}

// Create std.native.math module
ObjModule* create_std_math_module() {
    // Initialize random seed once if needed, or rely on user seeding.
    // We can seed on module creation.
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    ObjString* name = copyString("std.native.math", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "abs", native_abs);
    defineModuleFn(module, "ceil", native_ceil);
    defineModuleFn(module, "floor", native_floor);
    defineModuleFn(module, "round", native_round);
    defineModuleFn(module, "max", native_max);
    defineModuleFn(module, "min", native_min);
    defineModuleFn(module, "pow", native_pow);
    defineModuleFn(module, "sqrt", native_sqrt);
    defineModuleFn(module, "sin", native_sin);
    defineModuleFn(module, "cos", native_cos);
    defineModuleFn(module, "tan", native_tan);
    defineModuleFn(module, "asin", native_asin);
    defineModuleFn(module, "acos", native_acos);
    defineModuleFn(module, "atan", native_atan);
    defineModuleFn(module, "log", native_log);
    defineModuleFn(module, "exp", native_exp);
    defineModuleFn(module, "random", native_random);
    defineModuleFn(module, "randint", native_randint);
    defineModuleFn(module, "seed", native_seed);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}

// Register math functions as globals (for benchmarks/ease of use)
void register_math_globals(VM* vm) {
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
    defineNative(vm, "asin", native_asin);
    defineNative(vm, "acos", native_acos);
    defineNative(vm, "atan", native_atan);
    defineNative(vm, "log", native_log);
    defineNative(vm, "exp", native_exp);
    defineNative(vm, "random", native_random);
    defineNative(vm, "randint", native_randint);
    defineNative(vm, "seed", native_seed);
}
