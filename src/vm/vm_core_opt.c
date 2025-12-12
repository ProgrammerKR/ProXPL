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
        if (ip >= chunk->code_len) return 0;
        uint8_t op = chunk->code[ip++];

#if defined(__GNUC__) || defined(__clang__)
        /* fill common handlers once */
        dispatch_table[OP_NOP] = &&do_NOP;
        dispatch_table[OP_PUSH_CONST] = &&do_PUSH_CONST;
        dispatch_table[OP_CALL] = &&do_CALL;
        dispatch_table[OP_ADD] = &&do_ADD_FAST;
        dispatch_table[OP_HALT] = &&do_HALT;
        DISPATCH();

        do_NOP: { continue; }

        do_PUSH_CONST: {
            size_t read = 0;
            uint64_t idx = read_uleb128_from(chunk->code + ip, chunk->code_len - ip, &read);
            ip += read;
            stack[sp++] = consttable_get(&chunk->constants, (size_t)idx);
            continue;
        }

        /* Fast numeric add - type-specialized path */
        do_ADD_FAST: {
            if (unlikely(sp < 2)) return -1;
            Value b = stack[--sp];
            Value a = stack[--sp];
            if (likely(a.type == VAL_NUMBER && b.type == VAL_NUMBER)) {
                Value r; r.type = VAL_NUMBER; r.as.number = a.as.number + b.as.number;
                stack[sp++] = r; continue;
            }
            /* fallback to generic add handler - simple example */
            fprintf(stderr, "slow path: non-number add\n");
            return -1;
        }

        do_CALL: {
            /* inline cache lookup (very simple) */
            size_t saved_ip = ip;
            size_t read = 0;
            uint64_t idx = read_uleb128_from(chunk->code + ip, chunk->code_len - ip, &read);
            ip += read;
            if (ip >= chunk->code_len) return -1;
            uint8_t argc = chunk->code[ip++];

            Value callee = consttable_get(&chunk->constants, (size_t)idx);
            /* probe icache for site */
            uint32_t site = (uint32_t)(saved_ip & 63);
            ICEntry *ent = &icache[site];
            if (ent->site_pc == (uint32_t)saved_ip && ent->native_ptr) {
                ent->hit_count++;
                /* call native_ptr directly (demo only) */
                typedef Value (*native_fn)(Value*, int);
                native_fn fn = (native_fn)ent->native_ptr;
                Value ret = fn(&stack[sp-argc], (int)argc);
                sp -= argc; stack[sp++] = ret; continue;
            }

            /* resolve and fill cache (demo: only 'print') */
            if (callee.type == VAL_STRING && strcmp(callee.as.string.chars, "print") == 0) {
                /* cache a sentinel to native print handler */
                ent->site_pc = (uint32_t)saved_ip;
                ent->target_name = callee.as.string.chars;
                ent->native_ptr = NULL; /* optionally assign a function pointer */
                /* fallback: implement print directly */
                for (int i = (int)argc - 1; i >= 0; --i) {
                    Value arg = stack[--sp];
                    if (arg.type == VAL_STRING) fputs(arg.as.string.chars, stdout);
                    else if (arg.type == VAL_NUMBER) printf("%g", arg.as.number);
                    else if (arg.type == VAL_BOOL) fputs(arg.as.boolean?"true":"false", stdout);
                    else fputs("<obj>", stdout);
                    if (i) putchar(' ');
                }
                putchar('\n');
                Value r = make_null_const(); stack[sp++] = r; continue;
            }

            fprintf(stderr, "unsupported call target\n"); return -1;
        }

        do_HALT: { return 0; }

#else
dispatch_switch:
        switch (op) {
            case OP_NOP: break;
            case OP_PUSH_CONST: {
                size_t read=0; uint64_t idx = read_uleb128_from(chunk->code+ip, chunk->code_len-ip, &read); ip+=read;
                stack[sp++] = consttable_get(&chunk->constants, (size_t)idx);
            } break;
            case OP_ADD: {
                if (sp<2) return -1; Value b = stack[--sp]; Value a = stack[--sp];
                if (a.type==VAL_NUMBER && b.type==VAL_NUMBER) { Value r; r.type=VAL_NUMBER; r.as.number=a.as.number+b.as.number; stack[sp++]=r; }
                else { fprintf(stderr,"slow path: non-number add\n"); return -1; }
            } break;
            case OP_CALL: {
                size_t read=0; uint64_t idx = read_uleb128_from(chunk->code+ip, chunk->code_len-ip, &read); ip+=read;
                if (ip>=chunk->code_len) return -1; uint8_t argc = chunk->code[ip++];
                Value callee = consttable_get(&chunk->constants,(size_t)idx);
                if (callee.type==VAL_STRING && strcmp(callee.as.string.chars,"print")==0) {
                    for (int i=(int)argc-1;i>=0;--i){ Value arg=stack[--sp]; if (arg.type==VAL_STRING) fputs(arg.as.string.chars, stdout); else if (arg.type==VAL_NUMBER) printf("%g",arg.as.number); else fputs("<obj>",stdout); if (i) putchar(' ');} putchar('\n'); Value r = make_null_const(); stack[sp++]=r; break;
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
