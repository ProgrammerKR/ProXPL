// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
//

/*
 * vm_core_opt.c
 *
 * Optimized VM core loop prototype for ProXPL.
 * - computed-goto dispatch (gcc/clang)
 * - inline caching skeleton for call targets/property lookup
 * - fast numeric path for binary arithmetic (type-specialized)
 *
 * This file is a drop-in experimental core; it is intentionally self-contained
 * and documents the optimizations to be integrated into the main VM.
 */

#include "../../include/bytecode.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include <stdio.h>
#include <string.h>

/* Inline cache entry for call targets / property access */
typedef struct {
    uint32_t site_pc; /* bytecode pc of the call/property */
    const char *target_name; /* cached name (for simple engines) */
    void *native_ptr; /* direct native function pointer if resolved */
    uint32_t hit_count;
} ICEntry;

/* Micro-optimization: expect/likely macros for branch prediction */
#if defined(__GNUC__)
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

/* Optimized dispatch; similar to vm_dispatch but with fast-paths and IC hooks */
int vm_execute_optimized(const Chunk *chunk) {
    size_t ip = 0;
    Value stack[2048];
    int sp = 0;

    /* small polymorphic inline cache for demonstration */
    ICEntry icache[64];
    memset(icache, 0, sizeof(icache));

#if defined(__GNUC__) || defined(__clang__)
    static void *dispatch_table[256] = { &&do_NOP };
#define DISPATCH() goto *dispatch_table[op]
#else
#define DISPATCH() goto dispatch_switch
#endif

    for (;;) {
        if (ip >= (size_t)chunk->count) return 0;
        uint8_t op = chunk->code[ip++];

#if defined(__GNUC__) || defined(__clang__)
        /* fill common handlers once */
        dispatch_table[OP_NOP] = &&do_NOP;
        dispatch_table[OP_CONSTANT] = &&do_CONSTANT; // Replaces previous OP_PUSH_CONST
        dispatch_table[OP_CALL] = &&do_CALL;
        dispatch_table[OP_ADD] = &&do_ADD_FAST;
        dispatch_table[OP_HALT] = &&do_HALT;
        DISPATCH();

        do_NOP: { continue; }

        do_CONSTANT: {
            size_t read = 0;
            // NOTE: OP_CONSTANT usually takes a 1-byte operand in current bytecode.h, 
            // but for safety/flexibility let's assume standard handling or check logic.
            // If OpCode says OP_CONSTANT uses 1 byte index:
            uint8_t idx = chunk->code[ip++];
            stack[sp++] = consttable_get(chunk, (size_t)idx);
            continue;
        }

        /* Fast numeric add - type-specialized path */
        do_ADD_FAST: {
            if (unlikely(sp < 2)) return -1;
            Value b = stack[--sp];
            Value a = stack[--sp];
            if (likely(IS_NUMBER(a) && IS_NUMBER(b))) {
                Value r = NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b));
                stack[sp++] = r; continue;
            }
            /* fallback to generic add handler - simple example */
            fprintf(stderr, "slow path: non-number add\n");
            return -1;
        }

        do_CALL: {
            /* inline cache lookup (very simple) */
            size_t saved_ip = ip;
            
            // Assume OP_CALL format: [OP_CALL] [ArgCount] (1 byte)
            // Note: This logic must match the compiler's emission for OP_CALL.
            // Earlier files hinted at variable length, but simple compiler usually emits [OP] [Byte].
            // If invalid, we return error.
            if (ip >= (size_t)chunk->count) return -1;
            uint8_t argc = chunk->code[ip++];
            
            // For this optimized core, we assume the callee is on the stack *before* args? 
            // Or is it a constant call? The 'vm_execute_simple' used const index.
            // Standard ProXPL usually pushes callee then args.
            // Let's assume standard stack: [Callee] [Arg1] ... [ArgN]
            // We peak at stack[sp - 1 - argc] to find callee.
            
            if (sp < argc + 1) return -1;
            Value callee = stack[sp - 1 - argc];

            /* probe icache for site */
            uint32_t site = (uint32_t)(saved_ip & 63);
            ICEntry *ent = &icache[site];
            if (ent->site_pc == (uint32_t)saved_ip && ent->native_ptr) {
                ent->hit_count++;
                /* call native_ptr directly (demo only) */
                typedef Value (*native_fn)(Value*, int);
                native_fn fn = (native_fn)ent->native_ptr;
                Value ret = fn(&stack[sp-argc], (int)argc);
                sp -= (argc + 1); // Pop callee too
                stack[sp++] = ret; continue;
            }

            /* resolve and fill cache (demo: only 'print') */
            if (IS_STRING(callee) && strcmp(AS_CSTRING(callee), "print") == 0) {
                /* cache a sentinel to native print handler */
                ent->site_pc = (uint32_t)saved_ip;
                ent->target_name = AS_CSTRING(callee);
                ent->native_ptr = NULL; /* optionally assign a function pointer */
                /* fallback: implement print directly */
                for (int i = (int)argc - 1; i >= 0; --i) {
                    Value arg = stack[--sp];
                    if (IS_STRING(arg)) fputs(AS_CSTRING(arg), stdout);
                    else if (IS_NUMBER(arg)) printf("%g", AS_NUMBER(arg));
                    else if (IS_BOOL(arg)) fputs(AS_BOOL(arg)?"true":"false", stdout);
                    else fputs("<obj>", stdout);
                    if (i) putchar(' ');
                }
                putchar('\n');
                sp--; // Pop callee
                stack[sp++] = NIL_VAL; continue;
            }

            fprintf(stderr, "unsupported call target\n"); return -1;
        }

        do_HALT: { return 0; }

#else
dispatch_switch:
        switch (op) {
            case OP_CONSTANT: {
                uint8_t idx = chunk->code[ip++];
                stack[sp++] = consttable_get(chunk, (size_t)idx);
            } break;
            case OP_ADD: {
                if (sp<2) return -1; Value b = stack[--sp]; Value a = stack[--sp];
                if (IS_NUMBER(a) && IS_NUMBER(b)) { 
                    stack[sp++] = NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b)); 
                } else { fprintf(stderr,"slow path: non-number add\n"); return -1; }
            } break;
            case OP_CALL: {
                if (ip>=(size_t)chunk->count) return -1; 
                uint8_t argc = chunk->code[ip++];
                if (sp < argc + 1) return -1;
                Value callee = stack[sp - 1 - argc];
                
                if (IS_STRING(callee) && strcmp(AS_CSTRING(callee),"print")==0) {
                    for (int i=(int)argc-1;i>=0;--i){ 
                        Value arg=stack[--sp]; 
                        if (IS_STRING(arg)) fputs(AS_CSTRING(arg), stdout); 
                        else if (IS_NUMBER(arg)) printf("%g",AS_NUMBER(arg)); 
                        else fputs("<obj>",stdout); 
                        if (i) putchar(' ');
                    } 
                    putchar('\n'); 
                    sp--; // Pop callee
                    stack[sp++] = NIL_VAL; 
                    break;
                }
                fprintf(stderr,"unsupported call target\n"); return -1;
            }
            case OP_HALT: return 0;
            default: fprintf(stderr,"unhandled opcode %u\n",op); return -1;
        }
#endif
    }

    return 0;
}
