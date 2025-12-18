// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_VM_H
#define PROX_VM_H

#include "table.h"

#define STACK_MAX 256

typedef struct {
  Chunk *chunk;
  u8 *ip; // Instruction Pointer
  Value stack[STACK_MAX];
  Value *stackTop;
  Table globals;
  Table strings;
  struct Obj *objects;
} VM;

extern VM vm;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM(VM *vm);
void freeVM(VM *vm);
InterpretResult interpret(VM *vm, Chunk *chunk);
void push(VM *vm, Value value);
Value pop(VM *vm);

#endif
