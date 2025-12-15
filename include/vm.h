#ifndef PROXPL_VM_H
#define PROXPL_VM_H

#include "chunk.h"
#include "value.h"
#include "table.h" 
#include "object.h" 

#define STACK_MAX 256

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  Value stack[STACK_MAX];
  Value* stackTop;
  Obj* objects; 
  Table strings; 
  Table globals; 
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

// Global VM instance
extern VM vm;

// Updated Signatures to accept VM* pointer
void initVM(VM* vm);
void freeVM(VM* vm);
InterpretResult interpret(VM* vm, const char* source);
void push(VM* vm, Value value);
Value pop(VM* vm);

// Native function helper (Now accepts VM* to match native code calls)
void defineNative(VM* vm, const char* name, NativeFn function);

#endif

