// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
//

/*
  Instruction handlers template.
  Each handler receives a VMState* and decoded operands.
  The template documents expected conventions and shows example implementations
  for several opcodes. In production this would be integrated into vm_dispatch
  with direct calls or inlined for performance.

  NOTE: This file is a template and compiles as a demonstration.
*/

#include "../../include/bytecode.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Re-declare a tiny subset of VM types used for handlers */
#define MAX_REGS 16
#define OPERAND_STACK_MAX 1024
typedef struct VMState {
    Chunk *chunk;
    size_t ip;
    Value regs[MAX_REGS];
    Value stack[OPERAND_STACK_MAX];
    int sp;
} VMState;

/* ABI: handlers operate on VMState* and return 0 on success, non-zero on fatal error. */

static inline void handler_push(VMState *vm, Value v) {
    if (vm->sp >= OPERAND_STACK_MAX) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    vm->stack[vm->sp++] = v;
}
static inline Value handler_pop(VMState *vm) {
    if (vm->sp <= 0) { return NIL_VAL; }
    return vm->stack[--vm->sp];
}

int handle_push_const(VMState *vm, uint64_t const_idx) {
    Value v = consttable_get(&vm->chunk->constants, (size_t)const_idx);
    handler_push(vm, v);
    return 0;
}

int handle_call_const(VMState *vm, uint64_t const_idx, uint8_t argc) {
    /* Simplified: call a native function by string name only (for template) */
    Value callee = consttable_get(&vm->chunk->constants, (size_t)const_idx);
    if (IS_STRING(callee) && strcmp(AS_CSTRING(callee), "print")==0) {
        for (int i = (int)argc - 1; i >= 0; --i) {
            Value arg = handler_pop(vm);
            if (IS_STRING(arg)) printf("%s", AS_CSTRING(arg));
            else if (IS_NUMBER(arg)) printf("%g", AS_NUMBER(arg));
            else if (IS_BOOL(arg)) printf(AS_BOOL(arg) ? "true":"false");
            else printf("<obj>");
            if (i) printf(" ");
        }
        printf("\n");
        /* push null return */
        handler_push(vm, NIL_VAL);
        return 0;
    }
    /* in full runtime, we'd check for function objects, bound functions, etc. */
    fprintf(stderr,"unsupported call target in template handler\n");
    return -1;
}

int handle_add_stack(VMState *vm) {
    Value b = handler_pop(vm);
    Value a = handler_pop(vm);
    if (IS_NUMBER(a) && IS_NUMBER(b)) {
        Value out = NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b));
        handler_push(vm, out);
        return 0;
    }
    return -1;
}

/* Register ops example */
int handle_load_reg(VMState *vm, uint8_t dest_reg, uint8_t src_reg) {
    if (dest_reg >= MAX_REGS || src_reg >= MAX_REGS) return -1;
    vm->regs[dest_reg] = vm->regs[src_reg];
    return 0;
}

int handle_store_reg(VMState *vm, uint8_t dest_reg, uint8_t src_reg) {
    if (dest_reg >= MAX_REGS || src_reg >= MAX_REGS) return -1;
    vm->regs[dest_reg] = vm->regs[src_reg];
    return 0;
}

/* Control flow handlers are typically implemented inlined by the dispatch loop.
   Example for JMP (relative sleb128 offset):
*/
int handle_jmp(VMState *vm, int64_t rel_offset) {
    /* vm->ip is expected to be at next instruction already; adjust */
    if ((int64_t)vm->ip + rel_offset < 0) return -1;
    vm->ip = (size_t)((int64_t)vm->ip + rel_offset);
    return 0;
}

/* Close upvalue (template): in a real runtime we'd relocate the captured value into a heap cell */
int handle_close_upvalue(VMState *vm, uint8_t reg_index) {
    (void)vm; (void)reg_index;
    /* template: no-op */
    return 0;
}

/* End of template handlers */
