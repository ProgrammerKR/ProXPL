/*
 * ProXPL V2: High-Performance Register VM
 * ---------------------------------------
 * This file implements the core execution loop using a register-based architecture.
 * Features:
 *  - NaN-Boxing for values (64-bit doubles + tags)
 *  - Computed Gotos for instruction dispatch (GCC/Clang)
 *  - Fixed-width 32-bit instructions
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// --- Configuration ---
#define PREDICTED_STACK_SIZE 1024
#define MAX_REGISTERS 256

// --- Type System (NaN Boxing) ---
typedef uint64_t Value;

#define QNAN     ((uint64_t)0x7ffc000000000000)
#define TAG_NIL   1 // 01.
#define TAG_FALSE 2 // 10.
#define TAG_TRUE  3 // 11.

#define NIL_VAL         ((Value)(QNAN | TAG_NIL))
#define FALSE_VAL       ((Value)(QNAN | TAG_FALSE))
#define TRUE_VAL        ((Value)(QNAN | TAG_TRUE))

// Helper macros for value manipulation
#define IS_NUMBER(v)    (((v) & QNAN) != QNAN)
#define IS_NIL(v)       ((v) == NIL_VAL)
#define IS_BOOL(v)      (((v) | 1) == TRUE_VAL)

#define AS_NUMBER(v)    valueToNum(v)
#define NUMBER_VAL(n)   numToValue(n)

static inline double valueToNum(Value v) {
    union { uint64_t bits; double num; } u;
    u.bits = v;
    return u.num;
}

static inline Value numToValue(double n) {
    union { uint64_t bits; double num; } u;
    u.num = n;
    return u.bits;
}

// --- Instructions ---
// Format: | OpCode (8) | A (8) | B (8) | C (8) |
typedef uint32_t Instruction;

#define GET_OP(i)   ((i) & 0xFF)
#define GET_A(i)    (((i) >> 8) & 0xFF)
#define GET_B(i)    (((i) >> 16) & 0xFF)
#define GET_C(i)    (((i) >> 24) & 0xFF)

enum OpCode {
    OP_HALT = 0,
    OP_LOAD_CONST,
    OP_ADD,
    OP_RETURN,
    OP_MOV
};

// --- VM Structure ---
typedef struct {
    Instruction* ip;
    Value registers[MAX_REGISTERS];
    Value* constants;
    int const_count;
} VM;

// --- VM Execution Loop ---
// Returns: Exit code (0 = success)
int vm_run(VM* vm) {
    // Cache IP in register for speed
    register Instruction* ip = vm->ip;
    register Value* regs = vm->registers;

    #ifdef __GNUC__
        // Computed Goto Dispatch Table
        static void* dispatch_table[] = {
            &&HANDLE_HALT,
            &&HANDLE_LOAD_CONST,
            &&HANDLE_ADD,
            &&HANDLE_RETURN,
            &&HANDLE_MOV
        };
        #define DISPATCH() goto *dispatch_table[GET_OP(*ip)]
    #else
        // Switch Fallback
        #define DISPATCH() switch(GET_OP(*ip))
    #endif

    // Initial dispatch
    DISPATCH();

    // --- Opcode Handlers ---
    
    HANDLE_HALT:
    {
        return 0; 
    }

    HANDLE_LOAD_CONST:
    {
        Instruction ins = *ip++;
        uint8_t r_dest = GET_A(ins);
        uint8_t k_idx = GET_B(ins);
        regs[r_dest] = vm->constants[k_idx];
        
        #ifndef __GNUC__
        break;
        #endif
        DISPATCH();
    }

    HANDLE_ADD:
    {
        Instruction ins = *ip++;
        uint8_t r_dest = GET_A(ins);
        uint8_t r_b = GET_B(ins);
        uint8_t r_c = GET_C(ins);
        
        Value val_b = regs[r_b];
        Value val_c = regs[r_c];

        // Type specialization: Optimistic Float Add
        if (IS_NUMBER(val_b) && IS_NUMBER(val_c)) {
            double res = AS_NUMBER(val_b) + AS_NUMBER(val_c);
            regs[r_dest] = NUMBER_VAL(res);
        } else {
            // Fallback for strings/other types (stub)
            // runtime_add_generic(vm, r_dest, val_b, val_c);
            printf("Runtime Error: Invalid operands for ADD\n");
            return 1;
        }

        #ifndef __GNUC__
        break;
        #endif
        DISPATCH();
    }

    HANDLE_MOV:
    {
        Instruction ins = *ip++;
        uint8_t r_dest = GET_A(ins);
        uint8_t r_src = GET_B(ins);
        regs[r_dest] = regs[r_src];
        
        #ifndef __GNUC__
        break;
        #endif
        DISPATCH();
    }

    HANDLE_RETURN:
    {
        Instruction ins = *ip++; // consume
        uint8_t r_res = GET_A(ins);
        Value res = regs[r_res];
        
        // In real VM, this would pop frame or return to caller
        // For prototype, we print and exit
        if (IS_NUMBER(res)) {
            printf("VM Return: Number(%f)\n", AS_NUMBER(res));
        } else {
            printf("VM Return: Value(%llu)\n", res);
        }
        return 0;
    }
    
    // Switch closing brace if needed
    #ifndef __GNUC__
        } 
        // fallback loop for switch
    goto start_loop;
    #endif

    return 0;
}

// --- Test Main ---
int main() {
    printf("ProXPL V2 Register VM Prototype\n");
    printf("-------------------------------\n");

    // Constants
    Value consts[] = {
        NUMBER_VAL(10.0), // 0
        NUMBER_VAL(32.0), // 1
        NUMBER_VAL(5.5)   // 2
    };

    // Program: 10 + 32 = 42
    // R0 = 10.0
    // R1 = 32.0
    // R2 = R0 + R1
    // RETURN R2
    uint32_t code[] = {
        // Op | A | B | C
        (OP_LOAD_CONST) | (0 << 8) | (0 << 16),
        (OP_LOAD_CONST) | (1 << 8) | (1 << 16),
        (OP_ADD)        | (2 << 8) | (0 << 16) | (1 << 24),
        (OP_RETURN)     | (2 << 8)
    };

    VM vm;
    vm.ip = code;
    vm.constants = consts;
    vm.const_count = 3;
    
    vm_run(&vm);

    return 0;
}
