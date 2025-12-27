// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_VM_H
#define PROX_VM_H

#include "common.h"
#include "value.h"
#include "table.h" 
#include "importer.h"

#define FRAMES_MAX 256
#define STACK_MAX (FRAMES_MAX * 256)

// CallFrame is now defined in common.h

struct VM {
  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value* stackTop;
  Table globals;
  Table strings;
  Obj* objects;
  struct ObjUpvalue* openUpvalues;
  
  // GC State
  int grayCount;
  int grayCapacity;
  Obj** grayStack;
  size_t bytesAllocated;
  size_t nextGC;

  const char* source;
  Importer importer;
};

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern PROX_API VM vm;

void initVM(VM* vm);
void freeVM(VM* vm);
InterpretResult interpret(VM* vm, const char* source);
InterpretResult interpretChunk(VM* vm, Chunk* chunk);
InterpretResult interpretAST(VM* vm, StmtList* statements);
void push(VM* vm, Value value);
Value pop(VM* vm);
Value peek(VM* vm, int distance);
bool isFalsey(Value value);
void defineNative(VM* vm, const char* name, NativeFn function);
bool bindMethod(struct ObjClass *klass, struct ObjString *name, VM *vm);
void defineMethod(struct ObjString *name, VM *vm);
void closeUpvalues(VM *vm, Value *last);
struct ObjUpvalue *captureUpvalue(Value *local, VM *vm);
bool invokeFromClass(struct ObjClass *klass, struct ObjString *name, int argCount, VM *vm);
bool invoke(struct ObjString *name, int argCount, VM *vm);
bool callValue(Value callee, int argCount, VM *vm);

#endif // PROX_VM_H
