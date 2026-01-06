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

<<<<<<< HEAD
#define FRAMES_MAX 1024
=======
#define FRAMES_MAX 64
>>>>>>> fix-ci-build
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
<<<<<<< HEAD
  struct ObjUpvalue* openUpvalues;
=======
>>>>>>> fix-ci-build
  
  // GC State
  int grayCount;
  int grayCapacity;
  Obj** grayStack;
  size_t bytesAllocated;
  size_t nextGC;

  const char* source;
  Importer importer;
<<<<<<< HEAD
  struct ObjList* cliArgs;
  struct ObjString* initString;
=======
>>>>>>> fix-ci-build
};

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

<<<<<<< HEAD
extern PROX_API VM vm;
=======
extern VM vm;
>>>>>>> fix-ci-build

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
<<<<<<< HEAD
bool bindMethod(struct ObjClass *klass, struct ObjString *name, VM *vm);
void defineMethod(struct ObjString *name, VM *vm);
void closeUpvalues(VM *vm, Value *last);
struct ObjUpvalue *captureUpvalue(Value *local, VM *vm);
bool invokeFromClass(struct ObjClass *klass, struct ObjString *name, int argCount, VM *vm);
bool invoke(struct ObjString *name, int argCount, VM *vm);
bool callValue(Value callee, int argCount, VM *vm);
void runtimeError(VM* vm, const char* format, ...);
=======
>>>>>>> fix-ci-build

#endif // PROX_VM_H
