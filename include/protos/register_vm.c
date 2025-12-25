// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * protos/register_vm.c
 *
 * Simple register-based VM prototype.
 *  - 32-bit instruction: opcode:8 | dst:8 | a:8 | b:8
 *  - supports LOADI (imm8), ADD, SUB, MUL, DIV, PRINT, HALT
 *
 * Build:
 *   gcc -O2 -std=c11 -march=native -o register_vm protos/register_vm.c
 * Run:
 *   ./register_vm 50000000
 *
 * This is a minimal, immediately runnable prototype for benchmarking.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

typedef uint32_t instr_t;
typedef int64_t value_t;

enum {
    OP_NOP = 0,
    OP_LOADI,   // dst = imm8 (signed)
    OP_ADD,     // dst = a + b
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT,   // prints reg[dst]
    OP_HALT
};

#define OPCODE(i) ((uint8_t)((i) & 0xffu))
#define DST(i)    ((uint8_t)(((i) >> 8) & 0xffu))
#define AREG(i)   ((uint8_t)(((i) >> 16) & 0xffu))
#define BREG(i)   ((uint8_t)(((i) >> 24) & 0xffu))

static inline instr_t mk_instr(uint8_t op, uint8_t dst, uint8_t a, uint8_t b){
    return (instr_t)op | ((instr_t)dst << 8) | ((instr_t)a << 16) | ((instr_t)b << 24);
}

struct VM {
    value_t regs[256];      // 256 virtual registers
    instr_t *code;
    size_t code_len;
    size_t pc;
};

void run_vm(struct VM *vm){
    while (vm->pc < vm->code_len) {
        instr_t ins = vm->code[vm->pc++];
        switch (OPCODE(ins)) {
            case OP_NOP:
                break;
            case OP_LOADI: {
                // imm is in BREG field as signed int8_t
                int8_t imm = (int8_t)BREG(ins);
                vm->regs[DST(ins)] = (value_t)imm;
                break;
            }
            case OP_ADD: {
                uint8_t dst = DST(ins), a = AREG(ins), b = BREG(ins);
                vm->regs[dst] = vm->regs[a] + vm->regs[b];
                break;
            }
            case OP_SUB: {
                uint8_t dst = DST(ins), a = AREG(ins), b = BREG(ins);
                vm->regs[dst] = vm->regs[a] - vm->regs[b];
                break;
            }
            case OP_MUL: {
                uint8_t dst = DST(ins), a = AREG(ins), b = BREG(ins);
                vm->regs[dst] = vm->regs[a] * vm->regs[b];
                break;
            }
            case OP_DIV: {
                uint8_t dst = DST(ins), a = AREG(ins), b = BREG(ins);
                vm->regs[dst] = vm->regs[b] == 0 ? 0 : vm->regs[a] / vm->regs[b];
                break;
            }
            case OP_PRINT: {
                uint8_t dst = DST(ins);
                printf("%" PRId64 "\n", vm->regs[dst]);
                break;
            }
            case OP_HALT:
                return;
            default:
                fprintf(stderr,"Unknown opcode %u at pc %zu\n", OPCODE(ins), vm->pc-1);
                return;
        }
    }
}

/* Build a simple bytecode that computes:
 *
 *   r1 = 0
 *   for i in 0 .. N-1:
 *       r1 = r1 + i
 *
 * We implement the loop in C by generating the body and running many iterations.
 */

int main(int argc, char **argv){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <iterations>\n", argv[0]);
        return 1;
    }
    long long iterations = atoll(argv[1]);
    // We'll create a tiny program that sums 0..K-1 where K=iterations_per_inner
    // but to avoid huge code size we run the VM loop iterations times and inside the vm do a small inner sum.
    const int inner_k = 100; // sum 0..99 per VM run
    // program:
    // LOADI r0, 0         ; accumulator
    // LOADI r2, 0         ; counter
    // LOADI r3, inner_k   ; limit (fits in imm8 if <=127)
    // loop:
    // ADD r0, r0, r2
    // ADD r2, r2, 1
    // SUB r4, r3, r2
    // if r4 != 0 goto loop  (we don't have conditional jump here - emulate in outer loop)
    // PRINT r0
    // HALT

    // We will produce code for adding 0..inner_k-1 and return in r0.
    const size_t prog_len = 6;
    instr_t *prog = malloc(sizeof(instr_t) * prog_len);
    size_t ip = 0;
    prog[ip++] = mk_instr(OP_LOADI, 0, 0, 0);             // r0 = 0
    prog[ip++] = mk_instr(OP_LOADI, 2, 0, 0);             // r2 = 0
    prog[ip++] = mk_instr(OP_LOADI, 3, 0, (uint8_t)inner_k); // r3 = inner_k
    // loop unrolled: while r2 < r3: r0 += r2; r2 += 1
    // We'll unroll into fixed sequence inner_k times to keep VM simple:
    for (int i = 0; i < inner_k; ++i) {
        // ADD r0, r0, r2
        prog = realloc(prog, sizeof(instr_t) * (ip + 3 + 1)); // ensure space
        prog[ip++] = mk_instr(OP_ADD, 0, 0, 2);
        // ADD r2, r2, 1   -> LOADI tmp,1 ; ADD r2,r2,tmp
        prog[ip++] = mk_instr(OP_LOADI, 5, 0, 1); // r5 = 1
        prog[ip++] = mk_instr(OP_ADD, 2, 2, 5);
    }
    prog[ip++] = mk_instr(OP_PRINT, 0, 0, 0);
    prog[ip++] = mk_instr(OP_HALT, 0, 0, 0);

    struct VM vm = {0};
    vm.code = prog;
    vm.code_len = ip;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (long long iter = 0; iter < iterations; ++iter) {
        // reset registers used (0,2,3,5)
        vm.regs[0] = vm.regs[2] = vm.regs[3] = vm.regs[5] = 0;
        vm.pc = 0;
        // run but avoid printing on every iteration; we could conditionally print once
        // to verify correctness, only print on first iteration
        if (iter == 0) {
            run_vm(&vm); // will print sum for inner_k
        } else {
            // suppress printing by temporarily replacing PRINT with NOP
            // simple approach: patch the PRINT instr in prog to NOP for faster runs
            instr_t saved = vm.code[vm.code_len-2]; // second last is PRINT
            vm.code[vm.code_len-2] = mk_instr(OP_NOP,0,0,0);
            run_vm(&vm);
            vm.code[vm.code_len-2] = saved;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec)/1e9;
    fprintf(stderr, "Completed %lld iterations (inner_k=%d) in %.6fs\n", iterations, inner_k, elapsed);
    free(prog);
    return 0;
}
