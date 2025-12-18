// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

void freeObjects();

void initVM(VM *vm) { vm->stackTop = vm->stack; }

void freeVM(VM *vm) {
  // Nothing to free yet
}

void push(VM *vm, Value value) {
  *vm->stackTop = value;
  vm->stackTop++;
}

Value pop(VM *vm) {
  vm->stackTop--;
  return *vm->stackTop;
}

static InterpretResult run(VM *vm) {
  register u8* ip = vm->ip;
  register Value* sp = vm->stackTop;

#ifdef __GNUC__
  #define READ_BYTE() (*ip++)
  #define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
  #define DISPATCH() goto *dispatch_table[*ip++]
  
  static void* dispatch_table[] = {
      &&DO_OP_CONSTANT, &&DO_OP_TRUE, &&DO_OP_FALSE, &&DO_OP_NULL,
      &&DO_OP_POP, &&DO_OP_ADD, &&DO_OP_SUBTRACT, &&DO_OP_MULTIPLY, 
      &&DO_OP_DIVIDE, &&DO_OP_NOT, &&DO_OP_NEGATE, &&DO_OP_PRINT,
      &&DO_OP_RETURN
  };

  DISPATCH();

  DO_OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      *sp++ = constant;
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
  DO_OP_NULL: {
      *sp++ = NULL_VAL;
      DISPATCH();
  }
  DO_OP_POP: {
      sp--;
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
      *(sp-1) = BOOL_VAL(IS_NULL(v) || (IS_BOOL(v) && !AS_BOOL(v)));
      DISPATCH();
  }
  DO_OP_NEGATE: {
      double a = AS_NUMBER(*(sp-1));
      *(sp-1) = NUMBER_VAL(-a);
      DISPATCH();
  }
  DO_OP_PRINT: {
    // printValue expects Value, we need to sync slightly or safe call
    // printValue doesn't use VM, so it's safe.
    printValue(*(sp-1));
    sp--;
    printf("\n");
    DISPATCH();
  }
  DO_OP_RETURN: {
      vm->stackTop = sp; 
      vm->ip = ip;
      return INTERPRET_OK;
  }
  
#else
  // Fallback for MSVC / Standard C
  // We use local variables for speed even in switch
  vm->ip = ip;
  vm->stackTop = sp;
  
  #define READ_BYTE() (*vm->ip++)
  #define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
  #define BINARY_OP(valueType, op)                                               \
    do {                                                                         \
      if (!IS_NUMBER((*(vm->stackTop - 1))) ||                                   \
          !IS_NUMBER((*(vm->stackTop - 2)))) {                                   \
        printf("Operands must be numbers.\n");                                   \
        return INTERPRET_RUNTIME_ERROR;                                          \
      }                                                                          \
      double b = AS_NUMBER(pop(vm));                                             \
      double a = AS_NUMBER(pop(vm));                                             \
      push(vm, valueType(a op b));                                               \
    } while (false)

  for (;;) {
    u8 instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(vm, constant);
      break;
    }
    case OP_TRUE:
      push(vm, BOOL_VAL(true));
      break;
    case OP_FALSE:
      push(vm, BOOL_VAL(false));
      break;
    case OP_NULL:
      push(vm, NULL_VAL);
      break;
    case OP_POP:
      pop(vm);
      break;
    case OP_ADD:
      BINARY_OP(NUMBER_VAL, +);
      break;
    case OP_SUBTRACT:
      BINARY_OP(NUMBER_VAL, -);
      break;
    case OP_MULTIPLY:
      BINARY_OP(NUMBER_VAL, *);
      break;
    case OP_DIVIDE:
      BINARY_OP(NUMBER_VAL, /);
      break;
    case OP_NOT:
      push(vm, BOOL_VAL(IS_NULL(*(vm->stackTop - 1)) ||
                        (IS_BOOL(*(vm->stackTop - 1)) &&
                         !AS_BOOL(*(vm->stackTop - 1)))));
      break;
    case OP_NEGATE:
      if (!IS_NUMBER(*(vm->stackTop - 1))) {
        printf("Operand must be a number.\n");
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
    }
  }
#endif

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
#undef DISPATCH
}

InterpretResult interpret(VM *vm, Chunk *chunk) {
  vm->chunk = chunk;
  vm->ip = vm->chunk->code;
  return run(vm);
}
