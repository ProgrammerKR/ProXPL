#ifndef PROXPL_VM_H
#define PROXPL_VM_H

#include "chunk.h"
#include "value.h"
#include "table.h" // Agar table.h nahi hai to isse hata dein, but usually chahiye hota hai
#include "object.h" 

#define STACK_MAX 256

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  Value stack[STACK_MAX];
  Value* stackTop;
  Obj* objects; // <--- YE MISSING THA (Bahut Zaroori)
  // Table strings;  // Future ke liye
  // Table globals;  // Future ke liye
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
InterpretResult interpret(Chunk* chunk);
void push(Value value);
Value pop();

// Native function definition (Needed for convert_native.c)
void defineNative(VM* vm, const char* name, NativeFn function);

#endif

