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
  Obj* objects; // <--- This was missing and causing errors in object.c
  Table strings; // Needed for string interning
  Table globals; // Needed for global variables
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

// Global VM instance
extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source); // Standard CLOX signature
void push(Value value);
Value pop();

// Native function helper
void defineNative(const char* name, NativeFn function);

#endif

