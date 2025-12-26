// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "../include/common.h"
#include "../include/compiler.h"
#include "../include/debug.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/gc.h"
#include "../include/vm.h"
#include "../include/error_report.h"

VM vm;

static void resetStack(VM *pvm) {
  pvm->stackTop = pvm->stack;
  pvm->frameCount = 0;
}

void initVM(VM *pvm) { 
    resetStack(pvm);
    pvm->objects = NULL;
    initGC(pvm); // CRITICAL: Initialize GC state
    initTable(&pvm->globals);
    initTable(&pvm->strings);
    pvm->source = NULL;
    initImporter(&pvm->importer);
}

void freeVM(VM *pvm) {
  freeTable(&pvm->globals);
  freeTable(&pvm->strings);
  freeImporter(&pvm->importer);
  freeObjects(pvm);
}

// Runtime Error Helper
static void runtimeError(VM* pvm, const char* format, ...) {
  char message[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  CallFrame* frame = &pvm->frames[pvm->frameCount - 1];
  ObjFunction* function = frame->closure->function;
  size_t instruction = frame->ip - function->chunk.code - 1;
  int line = function->chunk.lines[instruction];

  reportRuntimeError(pvm->source, line, message);

  for (int i = pvm->frameCount - 1; i >= 0; i--) {
    CallFrame* frame = &pvm->frames[i];
    ObjFunction* function = frame->closure->function;
    size_t instruction = frame->ip - function->chunk.code - 1;
    fprintf(stderr, "  [line %d] in ", function->chunk.lines[instruction]);
    if (function->name == NULL) {
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", function->name->chars);
    }
  }

  resetStack(pvm);
}

void push(VM* pvm, Value value) {
  *pvm->stackTop = value;
  pvm->stackTop++;
}

Value pop(VM* pvm) {
  pvm->stackTop--;
  return *pvm->stackTop;
}

Value peek(VM* pvm, int distance) {
  return pvm->stackTop[-1 - distance];
}

bool isFalsey(Value value) {
  return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate(VM* pvm) {
  ObjString* b = AS_STRING(peek(pvm, 0));
  ObjString* a = AS_STRING(peek(pvm, 1));

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  pop(pvm);
  pop(pvm);
  push(pvm, OBJ_VAL(result));
}

static InterpretResult run(VM *vm) {
  CallFrame* frame = &vm->frames[vm->frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

#ifdef __GNUC__
  #define DISPATCH() goto *dispatch_table[*frame->ip++]
  
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
      push(vm, READ_CONSTANT());
      DISPATCH();
  }
  DO_OP_NIL: {
      push(vm, NULL_VAL);
      DISPATCH();
  }
  DO_OP_TRUE: {
      push(vm, BOOL_VAL(true));
      DISPATCH();
  }
  DO_OP_FALSE: {
      push(vm, BOOL_VAL(false));
      DISPATCH();
  }
  DO_OP_POP: {
      pop(vm);
      DISPATCH();
  }
  DO_OP_GET_LOCAL: {
      uint8_t slot = READ_BYTE();
      push(vm, frame->slots[slot]);
      DISPATCH();
  }
  DO_OP_SET_LOCAL: {
      uint8_t slot = READ_BYTE();
      frame->slots[slot] = peek(vm, 0);
      DISPATCH();
  }
  DO_OP_GET_GLOBAL: {
      ObjString* name = READ_STRING();
      Value value;
      if (!tableGet(&vm->globals, name, &value)) {
        runtimeError(vm, "Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      push(vm, value);
      DISPATCH();
  }
  DO_OP_DEFINE_GLOBAL: {
      ObjString* name = READ_STRING();
      tableSet(&vm->globals, name, peek(vm, 0));
      pop(vm);
      DISPATCH();
  }
  DO_OP_SET_GLOBAL: {
      ObjString* name = READ_STRING();
      if (tableSet(&vm->globals, name, peek(vm, 0))) {
        tableDelete(&vm->globals, name);
        runtimeError(vm, "Undefined variable '%s'.", name->chars);
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
      Value b = pop(vm);
      Value a = pop(vm);
      push(vm, BOOL_VAL(a == b)); 
      DISPATCH();
  }
  DO_OP_GREATER: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, BOOL_VAL(a > b));
      DISPATCH();
  }
  DO_OP_LESS: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, BOOL_VAL(a < b));
      DISPATCH();
  }
  DO_OP_ADD: {
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
      DISPATCH();
  }
  DO_OP_SUBTRACT: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(a - b));
      DISPATCH();
  }
  DO_OP_MULTIPLY: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(a * b));
      DISPATCH();
  }
  DO_OP_DIVIDE: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(a / b));
      DISPATCH();
  }
  DO_OP_NOT: {
      push(vm, BOOL_VAL(isFalsey(pop(vm))));
      DISPATCH();
  }
  DO_OP_NEGATE: {
      if (!IS_NUMBER(peek(vm, 0))) {
        runtimeError(vm, "Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm))));
      DISPATCH();
  }
  DO_OP_PRINT: {
    printValue(pop(vm));
    printf("\n");
    DISPATCH();
  }
  DO_OP_JUMP: {
      uint16_t offset = READ_SHORT();
      frame->ip += offset;
      DISPATCH();
  }
  DO_OP_JUMP_IF_FALSE: {
      uint16_t offset = READ_SHORT();
      if (isFalsey(peek(vm, 0))) frame->ip += offset;
      DISPATCH();
  }
  DO_OP_LOOP: {
      uint16_t offset = READ_SHORT();
      frame->ip -= offset;
      DISPATCH();
  }
  DO_OP_CALL: {
      int argCount = READ_BYTE();
      // callValue logic
      Value callee = peek(vm, argCount);
      if (IS_CLOSURE(callee)) {
          ObjClosure* closure = AS_CLOSURE(callee);
          if (argCount != closure->function->arity) {
              runtimeError(vm, "Expected %d arguments but got %d.", closure->function->arity, argCount);
              return INTERPRET_RUNTIME_ERROR;
          }
          if (vm->frameCount == 64) { // MAX_FRAMES
              runtimeError(vm, "Stack overflow.");
              return INTERPRET_RUNTIME_ERROR;
          }
          frame = &vm->frames[vm->frameCount++];
          frame->closure = closure;
          frame->ip = closure->function->chunk.code;
          frame->slots = vm->stackTop - argCount - 1;
          DISPATCH();
      } else if (IS_NATIVE(callee)) {
          NativeFn native = AS_NATIVE(callee);
          Value result = native(argCount, vm->stackTop - argCount);
          vm->stackTop -= argCount + 1;
          push(vm, result);
          DISPATCH();
      } else {
          runtimeError(vm, "Can only call functions and classes.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_INVOKE:
  DO_OP_SUPER_INVOKE:
  DO_OP_CLOSURE: {
      ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
      ObjClosure* closure = newClosure(function);
      push(vm, OBJ_VAL(closure));
      DISPATCH();
  }
  DO_OP_CLOSE_UPVALUE: {
      // Stubs
      DISPATCH();
  }
  DO_OP_RETURN: {
      Value result = pop(vm);
      vm->frameCount--;
      if (vm->frameCount == 0) {
        pop(vm);
        return INTERPRET_OK;
      }
      vm->stackTop = frame->slots;
      push(vm, result);
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
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
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: push(vm, READ_CONSTANT()); break;
    case OP_NIL: push(vm, NULL_VAL); break;
    case OP_TRUE: push(vm, BOOL_VAL(true)); break;
    case OP_FALSE: push(vm, BOOL_VAL(false)); break;
    case OP_POP: pop(vm); break;
    case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(vm, frame->slots[slot]);
        break;
    }
    case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        frame->slots[slot] = peek(vm, 0);
        break;
    }
    case OP_GET_GLOBAL: {
        ObjString* name = READ_STRING();
        Value value;
        if (!tableGet(&vm->globals, name, &value)) {
            runtimeError(vm, "Undefined variable '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
        }
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
            runtimeError(vm, "Undefined variable '%s'.", name->chars);
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
    case OP_NEGATE: {
        if (!IS_NUMBER(peek(vm, 0))) {
            runtimeError(vm, "Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
        }
        push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm)))); 
        break;
    }
    case OP_PRINT: {
      printValue(pop(vm));
      printf("\n");
      break;
    }
    case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        frame->ip += offset;
        break;
    }
    case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        if (isFalsey(peek(vm, 0))) frame->ip += offset;
        break;
    }
    case OP_LOOP: {
        uint16_t offset = READ_SHORT();
        frame->ip -= offset;
        break;
    }
    case OP_CALL: {
        int argCount = READ_BYTE();
        Value callee = peek(vm, argCount);
        if (IS_CLOSURE(callee)) {
            ObjClosure* closure = AS_CLOSURE(callee);
            if (argCount != closure->function->arity) {
                runtimeError(vm, "Expected %d arguments but got %d.", closure->function->arity, argCount);
                return INTERPRET_RUNTIME_ERROR;
            }
            if (vm->frameCount == 64) {
                runtimeError(vm, "Stack overflow.");
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm->frames[vm->frameCount++];
            frame->closure = closure;
            frame->ip = closure->function->chunk.code;
            frame->slots = vm->stackTop - argCount - 1;
            break;
        } else if (IS_NATIVE(callee)) {
            NativeFn native = AS_NATIVE(callee);
            Value result = native(argCount, vm->stackTop - argCount);
            vm->stackTop -= argCount + 1;
            push(vm, result);
            break;
        } else {
            runtimeError(vm, "Can only call functions and classes.");
            return INTERPRET_RUNTIME_ERROR;
        }
    }
    case OP_CLOSURE: {
        ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
        ObjClosure* closure = newClosure(function);
        push(vm, OBJ_VAL(closure));
        break;
    }
    case OP_RETURN: {
      vm->frameCount--;
      if (vm->frameCount == 0) {
        pop(vm);
        return INTERPRET_OK;
      }
      vm->stackTop = frame->slots;
      frame = &vm->frames[vm->frameCount - 1];
      break;
    }
    default: return INTERPRET_COMPILE_ERROR;
    }
  }
#endif

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef DISPATCH
}

InterpretResult interpretAST(VM* pvm, StmtList* statements) {
  // Disable GC during compilation to prevent freeing unrooted function/constants
  size_t oldNextGC = pvm->nextGC;
  pvm->nextGC = (size_t)-1; // SIZE_MAX

  ObjFunction* function = newFunction();
  
  // Connect the AST-based bytecode generator
  generateBytecode(statements, &function->chunk);
  
  // Setup for execution
  push(pvm, OBJ_VAL(function));
  ObjClosure* closure = newClosure(function);
  pop(pvm);
  push(pvm, OBJ_VAL(closure));
  
  // Restore GC
  pvm->nextGC = oldNextGC;
  
  CallFrame* frame = &pvm->frames[pvm->frameCount++];
  frame->closure = closure;
  frame->ip = function->chunk.code;
  frame->slots = pvm->stack;

  InterpretResult result = run(pvm);

  return result;
}

InterpretResult interpret(VM* pvm, const char* source) {
  pvm->source = source;
  ObjFunction* function = compile(source);
  if (function == NULL) return INTERPRET_COMPILE_ERROR;

  push(pvm, OBJ_VAL(function));
  ObjClosure* closure = newClosure(function);
  pop(pvm);
  push(pvm, OBJ_VAL(closure));

  CallFrame* frame = &pvm->frames[pvm->frameCount++];
  frame->closure = closure;
  frame->ip = function->chunk.code;
  frame->slots = pvm->stack;

  InterpretResult result = run(pvm);

  return result;
}

void defineNative(VM* vm, const char* name, NativeFn function) {
  push(vm, OBJ_VAL(copyString(name, (int)strlen(name))));
  push(vm, OBJ_VAL(newNative(function)));
  tableSet(&vm->globals, AS_STRING(vm->stackTop[-2]), vm->stackTop[-1]);
  pop(vm);
  pop(vm);
}

InterpretResult interpretChunk(VM* pvm, Chunk* chunk) {
    // Wrap the raw chunk into a function/closure
    ObjFunction* function = newFunction();
    function->chunk = *chunk; 
    
    // NOTE: This assumes ownership transfer or temporary usage.
    // Ensure we don't double free if main.c owns chunk.
    // For REPL, main.c inits chunk. function->chunk is a shallow copy.
    // function destructor will try to free chunk.
    // We should zero out function->chunk before returning or accept double-free risk?
    // Proper fix: `interpretChunk` should take compiled function, not chunk.
    // But to unblock REPL:
    
    push(pvm, OBJ_VAL(function));
    ObjClosure* closure = newClosure(function);
    pop(pvm);
    push(pvm, OBJ_VAL(closure));
    
    CallFrame* frame = &pvm->frames[pvm->frameCount++];
    frame->closure = closure;
    frame->ip = function->chunk.code;
    frame->slots = pvm->stack;
    
    InterpretResult result = run(pvm);
    
    // Prevent double free of chunk code/constants by function destructor
    // if main.c manages them.
    initChunk(&function->chunk);
    
    return result;
}

