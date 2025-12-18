<<<<<<< HEAD
/* src/runtime/vm.c */
#include <stdarg.h>
=======
// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

>>>>>>> feature/opcode-tests
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/common.h"
#include "../include/compiler.h"
#include "../include/debug.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/vm.h"
#include "../include/table.h"

<<<<<<< HEAD
// Helper to reset the stack
static void resetStack(VM* vm) {
  vm->stackTop = vm->stack;
=======
void freeObjects();

#include "table.h"
#include "object.h"

VM vm;

void initVM(VM *pvm) { 
    pvm->stackTop = pvm->stack; 
    pvm->objects = NULL;
    initTable(&pvm->globals);
    initTable(&pvm->strings);
}

void freeVM(VM *vm) {
  freeTable(&vm->globals);
  freeTable(&vm->strings);
  // freeObjects();
>>>>>>> feature/opcode-tests
}

// Runtime Error Helper
static void runtimeError(VM* vm, const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm->ip - vm->chunk->code - 1;
  int line = vm->chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack(vm);
}

// Initialize the VM
void initVM(VM* vm) {
  resetStack(vm);
  vm->objects = NULL;
  initTable(&vm->globals);
  initTable(&vm->strings);
  
  // You can register native functions here if you want them globally available
  // defineNative(vm, "clock", clockNative); 
}

// Free the VM resources
void freeVM(VM* vm) {
  freeTable(&vm->globals);
  freeTable(&vm->strings);
  freeObjects(); // Frees the linked list of objects
}

void push(VM* vm, Value value) {
  *vm->stackTop = value;
  vm->stackTop++;
}

Value pop(VM* vm) {
  vm->stackTop--;
  return *vm->stackTop;
}

static Value peek(VM* vm, int distance) {
  return vm->stackTop[-1 - distance];
}

<<<<<<< HEAD
// Helper to check for false/nil
static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

// Concatenate two strings
static void concatenate(VM* vm) {
  ObjString* b = AS_STRING(peek(vm, 0));
  ObjString* a = AS_STRING(peek(vm, 1));

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  pop(vm);
  pop(vm);
  push(vm, OBJ_VAL(result));
}

// Define a native function
void defineNative(VM* vm, const char* name, NativeFn function) {
  push(vm, OBJ_VAL(copyString(name, (int)strlen(name))));
  push(vm, OBJ_VAL(newNative(function)));
  tableSet(&vm->globals, AS_STRING(vm->stack[0]), vm->stack[1]);
  pop(vm);
  pop(vm);
}

// Main execution loop
static InterpretResult run(VM* vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) { \
        runtimeError(vm, "Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop(vm)); \
      double a = AS_NUMBER(pop(vm)); \
      push(vm, valueType(a op b)); \
    } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm->stack; slot < vm->stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(vm, constant);
        break;
      }
      case OP_TRUE:  push(vm, BOOL_VAL(true)); break;
      case OP_FALSE: push(vm, BOOL_VAL(false)); break;
      case OP_POP:   pop(vm); break;
      case OP_NULL:  push(vm, NULL_VAL); break; 

      case OP_ADD: {
        if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          concatenate(vm);
        } else if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(pop(vm));
          double a = AS_NUMBER(pop(vm));
          push(vm, NUMBER_VAL(a + b));
        } else {
          runtimeError(vm, "Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
      case OP_NOT:
        push(vm, BOOL_VAL(isFalsey(pop(vm))));
        break;
      case OP_NEGATE:
        if (!IS_NUMBER(peek(vm, 0))) {
          runtimeError(vm, "Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm))));
        break;
      case OP_PRINT: {
        printValue(pop(vm));
        printf("\n");
        break;
      }
      case OP_RETURN: {
        return INTERPRET_OK;
      }
=======
static bool isFalsey(Value value) {
  return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static InterpretResult run(VM *vm) {
  register u8* ip = vm->ip;
  register Value* sp = vm->stackTop;

#define READ_BYTE() (*ip++)
#define READ_SHORT() (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

#ifdef __GNUC__
  #define DISPATCH() goto *dispatch_table[*ip++]
  
  static void* dispatch_table[] = {
      &&DO_OP_CONSTANT, &&DO_OP_NIL, &&DO_OP_TRUE, &&DO_OP_FALSE,
      &&DO_OP_POP, &&DO_OP_GET_LOCAL, &&DO_OP_SET_LOCAL, 
      &&DO_OP_GET_GLOBAL, &&DO_OP_DEFINE_GLOBAL, &&DO_OP_SET_GLOBAL,
      &&DO_OP_GET_UPVALUE, &&DO_OP_SET_UPVALUE,
      &&DO_OP_GET_PROPERTY, &&DO_OP_SET_PROPERTY, &&DO_OP_GET_SUPER,
      &&DO_OP_EQUAL, &&DO_OP_GREATER, &&DO_OP_LESS,
      &&DO_OP_ADD, &&DO_OP_SUBTRACT, &&DO_OP_MULTIPLY, 
      &&DO_OP_DIVIDE, &&DO_OP_NOT, &&DO_OP_NEGATE, &&DO_OP_PRINT,
      &&DO_OP_JUMP, &&DO_OP_JUMP_IF_FALSE, &&DO_OP_LOOP,
      &&DO_OP_CALL, &&DO_OP_INVOKE, &&DO_OP_SUPER_INVOKE,
      &&DO_OP_CLOSURE, &&DO_OP_CLOSE_UPVALUE,
      &&DO_OP_RETURN, &&DO_OP_CLASS, &&DO_OP_INHERIT, &&DO_OP_METHOD
  };

  DISPATCH();

  DO_OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      *sp++ = constant;
      DISPATCH();
  }
  DO_OP_NIL: {
      *sp++ = NULL_VAL;
      DISPATCH();
  }
  DO_OP_TRUE: {
      *sp++ = BOOL_VAL(true);
      DISPATCH();
  }
  DO_OP_FALSE: {
      *sp++ = BOOL_VAL(false);
      DISPATCH();
  }
  DO_OP_POP: {
      sp--;
      DISPATCH();
  }
  DO_OP_GET_LOCAL: {
      u8 slot = READ_BYTE();
      // Placeholder: will need frame-relative indexing
      DISPATCH();
  }
  DO_OP_SET_LOCAL: {
      u8 slot = READ_BYTE();
      DISPATCH();
  }
  DO_OP_GET_GLOBAL: {
      ObjString* name = READ_STRING();
      Value value;
      if (!tableGet(&vm->globals, name, &value)) {
        // runtimeError("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      *sp++ = value;
      DISPATCH();
  }
  DO_OP_DEFINE_GLOBAL: {
      ObjString* name = READ_STRING();
      tableSet(&vm->globals, name, *(sp-1));
      sp--;
      DISPATCH();
  }
  DO_OP_SET_GLOBAL: {
      ObjString* name = READ_STRING();
      if (tableSet(&vm->globals, name, *(sp-1))) {
        tableDelete(&vm->globals, name);
        // runtimeError("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_GET_UPVALUE:
  DO_OP_SET_UPVALUE:
  DO_OP_GET_PROPERTY:
  DO_OP_SET_PROPERTY:
  DO_OP_GET_SUPER: {
      // Stubs
      DISPATCH();
  }
  DO_OP_EQUAL: {
      Value b = *(--sp);
      Value a = *(--sp);
      *sp++ = BOOL_VAL(a == b); // Simplified for primitive types
      DISPATCH();
  }
  DO_OP_GREATER: {
      double b = AS_NUMBER(*(--sp));
      double a = AS_NUMBER(*(--sp));
      *sp++ = BOOL_VAL(a > b);
      DISPATCH();
  }
  DO_OP_LESS: {
      double b = AS_NUMBER(*(--sp));
      double a = AS_NUMBER(*(--sp));
      *sp++ = BOOL_VAL(a < b);
      DISPATCH();
  }
  DO_OP_ADD: {
      double b = AS_NUMBER(*(sp - 1));
      double a = AS_NUMBER(*(sp - 2));
      sp -= 2;
      *sp++ = NUMBER_VAL(a + b);
      DISPATCH();
  }
  DO_OP_SUBTRACT: {
      double b = AS_NUMBER(*(sp - 1));
      double a = AS_NUMBER(*(sp - 2));
      sp -= 2;
      *sp++ = NUMBER_VAL(a - b);
      DISPATCH();
  }
  DO_OP_MULTIPLY: {
      double b = AS_NUMBER(*(sp - 1));
      double a = AS_NUMBER(*(sp - 2));
      sp -= 2;
      *sp++ = NUMBER_VAL(a * b);
      DISPATCH();
  }
  DO_OP_DIVIDE: {
      double b = AS_NUMBER(*(sp - 1));
      double a = AS_NUMBER(*(sp - 2));
      sp -= 2;
      *sp++ = NUMBER_VAL(a / b);
      DISPATCH();
  }
  DO_OP_NOT: {
      Value v = *(sp-1);
      *(sp-1) = BOOL_VAL(isFalsey(v));
      DISPATCH();
  }
  DO_OP_NEGATE: {
      double a = AS_NUMBER(*(sp-1));
      *(sp-1) = NUMBER_VAL(-a);
      DISPATCH();
  }
  DO_OP_PRINT: {
    printValue(*(sp-1));
    sp--;
    printf("\n");
    DISPATCH();
  }
  DO_OP_JUMP: {
      uint16_t offset = READ_SHORT();
      ip += offset;
      DISPATCH();
  }
  DO_OP_JUMP_IF_FALSE: {
      uint16_t offset = READ_SHORT();
      if (isFalsey(*(sp-1))) ip += offset;
      DISPATCH();
  }
  DO_OP_LOOP: {
      uint16_t offset = READ_SHORT();
      ip -= offset;
      DISPATCH();
  }
  DO_OP_CALL:
  DO_OP_INVOKE:
  DO_OP_SUPER_INVOKE:
  DO_OP_CLOSURE:
  DO_OP_CLOSE_UPVALUE: {
      // Stubs
      DISPATCH();
  }
  DO_OP_RETURN: {
      vm->stackTop = sp; 
      vm->ip = ip;
      return INTERPRET_OK;
  }
  DO_OP_CLASS:
  DO_OP_INHERIT:
  DO_OP_METHOD: {
      // Stubs
      DISPATCH();
  }
  
#else
  // Fallback for MSVC / Standard C
  for (;;) {
    u8 instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: push(vm, READ_CONSTANT()); break;
    case OP_NIL: push(vm, NULL_VAL); break;
    case OP_TRUE: push(vm, BOOL_VAL(true)); break;
    case OP_FALSE: push(vm, BOOL_VAL(false)); break;
    case OP_POP: pop(vm); break;
    case OP_GET_GLOBAL: {
        ObjString* name = READ_STRING();
        Value value;
        if (!tableGet(&vm->globals, name, &value)) return INTERPRET_RUNTIME_ERROR;
        push(vm, value);
        break;
    }
    case OP_DEFINE_GLOBAL: {
        ObjString* name = READ_STRING();
        tableSet(&vm->globals, name, peek(vm, 0));
        pop(vm);
        break;
    }
    case OP_SET_GLOBAL: {
        ObjString* name = READ_STRING();
        if (tableSet(&vm->globals, name, peek(vm, 0))) {
            tableDelete(&vm->globals, name);
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_EQUAL: {
        Value b = pop(vm);
        Value a = pop(vm);
        push(vm, BOOL_VAL(a == b));
        break;
    }
    case OP_GREATER: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, BOOL_VAL(a > b));
        break;
    }
    case OP_LESS: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, BOOL_VAL(a < b));
        break;
    }
    case OP_ADD: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(a + b));
        break;
    }
    case OP_SUBTRACT: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(a - b));
        break;
    }
    case OP_MULTIPLY: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(a * b));
        break;
    }
    case OP_DIVIDE: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(a / b));
        break;
    }
    case OP_NOT: push(vm, BOOL_VAL(isFalsey(pop(vm)))); break;
    case OP_NEGATE: push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm)))); break;
    case OP_PRINT: {
      printValue(pop(vm));
      printf("\n");
      break;
    }
    case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        vm->ip += offset;
        break;
    }
    case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        if (isFalsey(peek(vm, 0))) vm->ip += offset;
        break;
    }
    case OP_LOOP: {
        uint16_t offset = READ_SHORT();
        vm->ip -= offset;
        break;
    }
    case OP_RETURN: return INTERPRET_OK;
    default: return INTERPRET_COMPILE_ERROR;
>>>>>>> feature/opcode-tests
    }
  }
#endif

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef DISPATCH
}

// Compile and Run
InterpretResult interpret(VM* vm, const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm->chunk = &chunk;
  vm->ip = vm->chunk->code;

  InterpretResult result = run(vm);

  freeChunk(&chunk);
  return result;
}

