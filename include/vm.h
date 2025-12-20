#ifndef PROXPL_VM_H
#define PROXPL_VM_H

#include "bytecode.h"
#include "value.h"
#include "table.h" 
#include "object.h" 

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * 256)

typedef struct {
  ObjFunction* function;
  uint8_t* ip;
  Value* slots;
} CallFrame;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value* stackTop;
  Table globals;
  Table strings;
  Obj* objects;

  const char* source; // Added for error reporting
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM(VM* vm);
void freeVM(VM* vm);
InterpretResult interpret(VM* vm, const char* source);
InterpretResult interpretAST(VM* vm, StmtList* statements);
void push(VM* vm, Value value);
Value pop(VM* vm);
void defineNative(VM* vm, const char* name, NativeFn function);

#endif

