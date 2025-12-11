/*
  Dispatch loop example implementing both computed-goto (for compilers
  supporting it) and switch fallback. This is a compact, clear demonstration;
  it does not implement a full VM, but shows the pattern and includes
  decoding logic for a subset of instructions.

  The dispatch loop expects a Chunk* with code and constants populated.
  It uses a simple operand stack of Value and a small register file.
*/

#include "../../include/bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Minimal VM runtime structures for demo */
#define MAX_REGS 16
#define OPERAND_STACK_MAX 1024

typedef struct {
    const Chunk *chunk;
    size_t ip; /* instruction pointer (index into chunk->code) */
    Value regs[MAX_REGS];
    Value stack[OPERAND_STACK_MAX];
    int sp; /* stack pointer: index of next free slot */
} VMState;

static void push_value(VMState *vm, Value v) {
    if (vm->sp >= OPERAND_STACK_MAX) { fprintf(stderr,"stack overflow\n"); exit(1); }
    vm->stack[vm->sp++] = v;
}
static Value pop_value(VMState *vm) {
    if (vm->sp <= 0) { Value n; n.type = VAL_NULL; return n; }
    return vm->stack[--vm->sp];
}

/* truthiness */
static int is_truthy(const Value *v) {
    if (!v) return 0;
    if (v->type == VAL_NULL) return 0;
    if (v->type == VAL_BOOL) return v->as.boolean;
    if (v->type == VAL_NUMBER) return v->as.number != 0.0;
    return 1;
}

/* decode helpers reading LEB128 from chunk */
static uint64_t read_uleb128_chunk(const Chunk *c, size_t *pos) {
    return read_uleb128_from(c->code + *pos, c->code_len - *pos, pos);
}
static int64_t read_sleb128_chunk(const Chunk *c, size_t *pos) {
    return read_sleb128_from(c->code + *pos, c->code_len - *pos, pos);
}

/* Helper to get const by uleb idx within chunk */
static Value get_const(const Chunk *c, uint64_t idx) {
    return consttable_get(&c->constants, (size_t)idx);
}

/* Simple runtime: implement only a few opcodes to validate dispatch */
static int vm_execute_simple(const Chunk *chunk) {
    VMState vm;
    memset(&vm,0,sizeof(vm));
    vm.chunk = chunk;
    vm.ip = 0;
    vm.sp = 0;

#if defined(__GNUC__) || defined(__clang__)
    /* computed goto table */
    static void *dispatch_table[256] = { &&do_NOP };
    /* initialize known op labels */
    /* NOTE: C requires labels to be in function scope; create them here */
    #define DISPATCH() goto *dispatch_table[opcode]
#else
    #define DISPATCH() goto dispatch_switch
#endif

    for (;;) {
        if (vm.ip >= chunk->code_len) { return 0; }
        uint8_t opcode = chunk->code[vm.ip++];

#if defined(__GNUC__) || defined(__clang__)
        /* lazy fill dispatch labels */
        /* The table entries are set below just once per function call. */
        dispatch_table[OP_NOP] = &&do_NOP;
        dispatch_table[OP_PUSH_CONST] = &&do_PUSH_CONST;
        dispatch_table[OP_CALL] = &&do_CALL;
        dispatch_table[OP_HALT] = &&do_HALT;
        dispatch_table[OP_ADD] = &&do_ADD;
        dispatch_table[OP_POP] = &&do_POP;
        /* jump to handler */
        DISPATCH();

        do_NOP: {
            /* no-op */
            continue;
        }
        do_PUSH_CONST: {
            size_t old = vm.ip;
            size_t read = 0;
            uint64_t idx = read_uleb128_from(chunk->code + vm.ip, chunk->code_len - vm.ip, &read);
            vm.ip += read;
            Value v = get_const(chunk, idx);
            push_value(&vm, v);
            continue;
        }
        do_CALL: {
            if (vm.ip >= chunk->code_len) return -1;
            uint8_t am = chunk->code[vm.ip++];
            if (am == AM_CONST) {
                size_t read=0;
                uint64_t idx = read_uleb128_from(chunk->code + vm.ip, chunk->code_len - vm.ip, &read);
                vm.ip += read;
                if (vm.ip >= chunk->code_len) return -1;
                uint8_t argc = chunk->code[vm.ip++];
                /* For test mode: support only native 'print' identified by string const "print" */
                Value callee = get_const(chunk, idx);
                if (callee.type == VAL_STRING && strcmp(callee.as.string.chars,"print")==0) {
                    /* pop argc args into an array, print them to stdout */
                    for (int i = (int)argc - 1; i >= 0; --i) {
                        Value arg = pop_value(&vm);
                        if (arg.type == VAL_STRING) {
                            printf("%s", arg.as.string.chars);
                        } else if (arg.type == VAL_NUMBER) {
                            printf("%g", arg.as.number);
                        } else if (arg.type == VAL_BOOL) {
                            printf(arg.as.boolean ? "true":"false");
                        } else if (arg.type == VAL_NULL) {
                            printf("null");
                        } else {
                            printf("<obj>");
                        }
                        if (i) printf(" ");
                    }
                    printf("\n");
                    /* push null as return */
                    Value r = make_null_const();
                    push_value(&vm, r);
                    continue;
                } else {
                    fprintf(stderr,"unsupported call target in test vm\n");
                    return -1;
                }
            } else {
                fprintf(stderr,"unsupported addressing mode in test vm\n");
                return -1;
            }
        }
        do_ADD: {
            if (vm.sp < 2) { fprintf(stderr,"stack underflow add\n"); return -1; }
            Value b = pop_value(&vm);
            Value a = pop_value(&vm);
            Value out;
            if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                out.type = VAL_NUMBER;
                out.as.number = a.as.number + b.as.number;
                push_value(&vm, out);
            } else {
                /* attempt to coerce or error */
                fprintf(stderr,"type error in ADD\n");
                return -1;
            }
            continue;
        }
        do_POP: {
            if (vm.sp > 0) vm.sp--;
            continue;
        }
        do_HALT: {
            return 0;
        }

#else
dispatch_switch:
        switch (opcode) {
            case OP_NOP: break;
            case OP_PUSH_CONST: {
                size_t read = 0;
                uint64_t idx = read_uleb128_from(chunk->code + vm.ip, chunk->code_len - vm.ip, &read);
                vm.ip += read;
                Value v = get_const(chunk, idx);
                push_value(&vm, v);
            } break;
            case OP_CALL: {
                if (vm.ip >= chunk->code_len) return -1;
                uint8_t am = chunk->code[vm.ip++];
                if (am == AM_CONST) {
                    size_t read = 0;
                    uint64_t idx = read_uleb128_from(chunk->code + vm.ip, chunk->code_len - vm.ip, &read);
                    vm.ip += read;
                    if (vm.ip >= chunk->code_len) return -1;
                    uint8_t argc = chunk->code[vm.ip++];
                    Value callee = get_const(chunk, idx);
                    if (callee.type == VAL_STRING && strcmp(callee.as.string.chars,"print")==0) {
                        for (int i = (int)argc - 1; i >= 0; --i) {
                            Value arg = pop_value(&vm);
                            if (arg.type == VAL_STRING) {
                                printf("%s", arg.as.string.chars);
                            } else if (arg.type == VAL_NUMBER) {
                                printf("%g", arg.as.number);
                            } else if (arg.type == VAL_BOOL) {
                                printf(arg.as.boolean ? "true":"false");
                            } else if (arg.type == VAL_NULL) {
                                printf("null");
                            } else {
                                printf("<obj>");
                            }
                            if (i) printf(" ");
                        }
                        printf("\n");
                        Value r = make_null_const();
                        push_value(&vm, r);
                    } else {
                        fprintf(stderr,"unsupported call target in test vm\n");
                        return -1;
                    }
                } else {
                    fprintf(stderr,"unsupported addressing mode in test vm\n");
                    return -1;
                }
            } break;
            case OP_ADD: {
                if (vm.sp < 2) { fprintf(stderr,"stack underflow add\n"); return -1; }
                Value b = pop_value(&vm);
                Value a = pop_value(&vm);
                Value out;
                if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                    out.type = VAL_NUMBER;
                    out.as.number = a.as.number + b.as.number;
                    push_value(&vm, out);
                } else {
                    fprintf(stderr,"type error in ADD\n");
                    return -1;
                }
            } break;
            case OP_POP:
                if (vm.sp>0) vm.sp--;
                break;
            case OP_HALT:
                return 0;
            default:
                fprintf(stderr,"unhandled opcode 0x%02X\n", opcode);
                return -1;
        } /* switch */
#endif

    } /* for */

    return 0;
}

/* Expose a simple test-runner function */
int vm_run_chunk_simple(const Chunk *chunk) {
    return vm_execute_simple(chunk);
}
