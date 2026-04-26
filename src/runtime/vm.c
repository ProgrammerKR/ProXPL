// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// Fixed LLVM and VM issues.
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h> // Required for exit()
#include <math.h>



#include "../include/common.h"
#include "../include/compiler.h"
#include "../include/debug.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/gc.h"
#include "../include/vm.h"
#include "../include/error_report.h"
#include "../include/ffi_bridge.h"


VM vm;

static void resetStack(VM *pvm) {
  pvm->stackTop = pvm->stack;
  pvm->frameCount = 0;
  // CRITICAL FIX: Initialize stack to prevent reading uninitialized memory
  // Without this, NaN-boxed Values can have corrupted type tags from random bits
  for (int i = 0; i < STACK_MAX; i++) {
    pvm->stack[i] = NIL_VAL;
  }
}

void initVM(VM *pvm) { 
    resetStack(pvm);
    pvm->objects = NULL;
    initGC(pvm); // CRITICAL: Initialize GC state
    initTable(&pvm->globals);
    initTable(&pvm->strings);
    pvm->source = NULL;
    initImporter(&pvm->importer);
    pvm->initString = copyString("init", 4);
    pvm->cliArgs = newList(); 
    pvm->activeContextCount = 0;
}

void freeVM(VM *pvm) {
  freeTable(&pvm->globals);
  freeTable(&pvm->strings);
  freeImporter(&pvm->importer);
  pvm->initString = NULL; // CRITICAL: Prevent use-after-free
  freeObjects(pvm);
}

// Runtime Error Helper
void runtimeError(VM* pvm, const char* format, ...) {
  char message[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  if (pvm->frameCount == 0) {
    fprintf(stderr, "%s\n", message);
    resetStack(pvm);
    return;
  }

  CallFrame* frame = &pvm->frames[pvm->frameCount - 1];
  ObjFunction* function = frame->closure->function;
  size_t instruction = frame->ip - function->chunk.code - 1;
  int line = function->chunk.lines[instruction];

  reportRuntimeError(pvm->source, line, message);

  for (int i = pvm->frameCount - 1; i >= 0; i--) {
    CallFrame* f = &pvm->frames[i];
    ObjFunction* fn = f->closure->function;
    size_t inst = f->ip - fn->chunk.code - 1;
    fprintf(stderr, "  [line %d] in ", fn->chunk.lines[inst]);
    if (fn->name == NULL) {
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", fn->name->chars);
    }
  }

  resetStack(pvm);
}

void push(VM* pvm, Value value) {
  if (pvm->stackTop >= pvm->stack + STACK_MAX) {
      // NOTE: Most stack checks now happen via the PUSH macro in run()
      // to allow recoverable errors. This function remains for external use.
      fprintf(stderr, "Fatal Runtime Error: Value stack overflow in push().\n");
      exit(1); // CRITICAL: Prevent memory corruption
  }
  *pvm->stackTop = value;
  pvm->stackTop++;
}

Value pop(VM* pvm) {
  if (pvm->stackTop <= pvm->stack) {
      fprintf(stderr, "Fatal Runtime Error: Value stack underflow in pop().\n");
      exit(1);
  }
  pvm->stackTop--;
  return *pvm->stackTop;
}

Value peek(VM* pvm, int distance) {
  return pvm->stackTop[-1 - distance];
}

bool isFalsey(Value value) {
  return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static bool performTensorArithmetic(VM* pvm, char op) {
    Value bVal = peek(pvm, 0);
    Value aVal = peek(pvm, 1);
    
    // Case 1: Tensor op Tensor
    if (IS_TENSOR(aVal) && IS_TENSOR(bVal)) {
        ObjTensor* a = AS_TENSOR(aVal);
        ObjTensor* b = AS_TENSOR(bVal);
        
        // Element-wise arithmetic requires matching size
        if (a->size != b->size) {
            runtimeError(pvm, "Tensor size mismatch: %d vs %d.", a->size, b->size);
            return true;
        }
        
        if (a->dimCount != b->dimCount) {
             runtimeError(pvm, "Tensor rank mismatch.");
             return true;
        }
        for (int i=0; i<a->dimCount; i++) {
             if (a->dims[i] != b->dims[i]) {
                 runtimeError(pvm, "Tensor dimension mismatch at axis %d: %d vs %d.", i, a->dims[i], b->dims[i]);
                 return true;
             }
        }
        
        ObjTensor* res = newTensor(a->dimCount, a->dims, NULL);
        push(pvm, OBJ_VAL(res)); 
        
        switch (op) {
            case '+': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] + b->data[i]; break;
            case '-': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] - b->data[i]; break;
            case '*': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] * b->data[i]; break;
            case '/': 
                for (int i = 0; i < a->size; i++) {
                    if (b->data[i] == 0) {
                        runtimeError(pvm, "Tensor division by zero.");
                        return true;
                    }
                    res->data[i] = a->data[i] / b->data[i];
                }
                break;
        }
        
        Value resVal = pop(pvm);
        pop(pvm); // b
        pop(pvm); // a
        push(pvm, resVal);
        return true;
    }
    
    // Case 2: Tensor op Scalar
    if (IS_TENSOR(aVal) && IS_NUMBER(bVal)) {
        ObjTensor* a = AS_TENSOR(aVal);
        double b = AS_NUMBER(bVal);
        
        ObjTensor* res = newTensor(a->dimCount, a->dims, NULL);
        push(pvm, OBJ_VAL(res));
        
        switch (op) {
            case '+': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] + b; break;
            case '-': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] - b; break;
            case '*': for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] * b; break;
            case '/': 
                if (b == 0) {
                    runtimeError(pvm, "Division by zero.");
                    return true;
                }
                for (int i = 0; i < a->size; i++) res->data[i] = a->data[i] / b; 
                break;
        }
        Value resVal = pop(pvm);
        pop(pvm); pop(pvm); push(pvm, resVal);
        return true;
    }
    
    // Case 3: Scalar op Tensor
    if (IS_NUMBER(aVal) && IS_TENSOR(bVal)) {
        double a = AS_NUMBER(aVal);
        ObjTensor* b = AS_TENSOR(bVal);
        
        ObjTensor* res = newTensor(b->dimCount, b->dims, NULL);
        push(pvm, OBJ_VAL(res));
        
        switch (op) {
            case '+': for (int i = 0; i < b->size; i++) res->data[i] = a + b->data[i]; break;
            case '-': for (int i = 0; i < b->size; i++) res->data[i] = a - b->data[i]; break;
            case '*': for (int i = 0; i < b->size; i++) res->data[i] = a * b->data[i]; break;
            case '/': 
                for (int i = 0; i < b->size; i++) {
                    if (b->data[i] == 0) {
                        runtimeError(pvm, "Division by zero.");
                        return true;
                    }
                    res->data[i] = a / b->data[i]; 
                }
                break;
        }
        Value resVal = pop(pvm);
        pop(pvm); pop(pvm); push(pvm, resVal);
        return true;
    }

    return false;
}

static void concatenate(VM* pvm) {
  ObjString* b = AS_STRING(peek(pvm, 0));
  ObjString* a = AS_STRING(peek(pvm, 1));

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  // a and b are still on stack, so they are rooted
  ObjString* result = takeString(chars, length);
  
  // Now we can safely replace a and b with the result
  pvm->stackTop[-2] = OBJ_VAL(result);
  pvm->stackTop--;
}

static bool resolveContextualMethod(VM* pvm, ObjString* name, Value* result) {
    for (int i = pvm->activeContextCount - 1; i >= 0; i--) {
        ObjContext* context = pvm->activeContextStack[i];
        
        // Iterating Table manually as we don't have a tableIterator helper
        for (int j = 0; j < context->layers.capacity; j++) {
            Entry* entry = &context->layers.entries[j];
            if (entry->key == NULL) continue;
            if (IS_BOOL(entry->value)) continue; // Tombstone
            
            ObjLayer* layer = AS_LAYER(entry->value);
            if (tableGet(&layer->methods, name, result)) {
                return true;
            }
        }
    }
    return false;
}

// Helper functions moved to vm_helpers.c to avoid duplication

static InterpretResult run(VM* pvm) {
#define vm pvm
  CallFrame* frame = &vm->frames[vm->frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

#define PUSH(value) \
    do { \
        if (vm->stackTop >= vm->stack + STACK_MAX) { \
            runtimeError(vm, "Stack overflow."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        *vm->stackTop++ = (value); \
    } while (false)

#ifdef __GNUC__
  #define DISPATCH() goto *dispatch_table[*frame->ip++]
  
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverride-init"
  static void* dispatch_table[256] = {
      [0 ... 255] = &&trap,
      [OP_CONSTANT] = &&DO_OP_CONSTANT,
      [OP_NOP] = &&DO_OP_NOP,
      [OP_NIL] = &&DO_OP_NIL,
      [OP_TRUE] = &&DO_OP_TRUE,
      [OP_FALSE] = &&DO_OP_FALSE,
      [OP_POP] = &&DO_OP_POP,
      [OP_DUP] = &&DO_OP_DUP,
      [OP_BUILD_LIST] = &&DO_OP_BUILD_LIST,
      [OP_BUILD_MAP] = &&DO_OP_BUILD_MAP,
      [OP_GET_INDEX] = &&DO_OP_GET_INDEX,
      [OP_SET_INDEX] = &&DO_OP_SET_INDEX,
      [OP_GET_LOCAL] = &&DO_OP_GET_LOCAL,
      [OP_GET_LOCAL_0] = &&DO_OP_GET_LOCAL_0,
      [OP_GET_LOCAL_1] = &&DO_OP_GET_LOCAL_1,
      [OP_GET_LOCAL_2] = &&DO_OP_GET_LOCAL_2,
      [OP_GET_LOCAL_3] = &&DO_OP_GET_LOCAL_3,
      [OP_SET_LOCAL] = &&DO_OP_SET_LOCAL,
      [OP_SET_LOCAL_0] = &&DO_OP_SET_LOCAL_0,
      [OP_SET_LOCAL_1] = &&DO_OP_SET_LOCAL_1,
      [OP_SET_LOCAL_2] = &&DO_OP_SET_LOCAL_2,
      [OP_SET_LOCAL_3] = &&DO_OP_SET_LOCAL_3,
      [OP_GET_GLOBAL] = &&DO_OP_GET_GLOBAL,
      [OP_DEFINE_GLOBAL] = &&DO_OP_DEFINE_GLOBAL,
      [OP_SET_GLOBAL] = &&DO_OP_SET_GLOBAL,
      [OP_GET_UPVALUE] = &&DO_OP_GET_UPVALUE,
      [OP_SET_UPVALUE] = &&DO_OP_SET_UPVALUE,
      [OP_GET_PROPERTY] = &&DO_OP_GET_PROPERTY,
      [OP_SET_PROPERTY] = &&DO_OP_SET_PROPERTY,
      [OP_GET_SUPER] = &&DO_OP_GET_SUPER,
      [OP_EQUAL] = &&DO_OP_EQUAL,
      [OP_GREATER] = &&DO_OP_GREATER,
      [OP_LESS] = &&DO_OP_LESS,
      [OP_ADD] = &&DO_OP_ADD,
      [OP_SUBTRACT] = &&DO_OP_SUBTRACT,
      [OP_MULTIPLY] = &&DO_OP_MULTIPLY,
      [OP_DIVIDE] = &&DO_OP_DIVIDE,
      [OP_NOT] = &&DO_OP_NOT,
      [OP_NEGATE] = &&DO_OP_NEGATE,
      [OP_PRINT] = &&DO_OP_PRINT,
      [OP_JUMP] = &&DO_OP_JUMP,
      [OP_JUMP_IF_FALSE] = &&DO_OP_JUMP_IF_FALSE,
      [OP_LOOP] = &&DO_OP_LOOP,
      [OP_CALL] = &&DO_OP_CALL,
      [OP_INVOKE] = &&DO_OP_INVOKE,
      [OP_SUPER_INVOKE] = &&DO_OP_SUPER_INVOKE,
      [OP_CLOSURE] = &&DO_OP_CLOSURE,
      [OP_CLOSE_UPVALUE] = &&DO_OP_CLOSE_UPVALUE,
      [OP_RETURN] = &&DO_OP_RETURN,
      [OP_CLASS] = &&DO_OP_CLASS,
      [OP_INHERIT] = &&DO_OP_INHERIT,
      [OP_METHOD] = &&DO_OP_METHOD,
      [OP_USE] = &&DO_OP_USE,
      [OP_TRY] = &&DO_OP_TRY,
      [OP_CATCH] = &&DO_OP_CATCH,
      [OP_END_TRY] = &&DO_OP_END_TRY,
      [OP_INTERFACE] = &&DO_OP_INTERFACE,
      [OP_IMPLEMENT] = &&DO_OP_IMPLEMENT,
      [OP_MAKE_FOREIGN] = &&DO_OP_MAKE_FOREIGN,
      [OP_MODULO] = &&DO_OP_MODULO,
      [OP_BIT_AND] = &&DO_OP_BIT_AND,
      [OP_BIT_OR] = &&DO_OP_BIT_OR,
      [OP_BIT_XOR] = &&DO_OP_BIT_XOR,
      [OP_BIT_NOT] = &&DO_OP_BIT_NOT,
      [OP_LEFT_SHIFT] = &&DO_OP_LEFT_SHIFT,
      [OP_RIGHT_SHIFT] = &&DO_OP_RIGHT_SHIFT,
      [OP_MAT_MUL] = &&DO_OP_MAT_MUL,
      [OP_MAKE_TENSOR] = &&DO_OP_MAKE_TENSOR,
      [OP_CONTEXT] = &&DO_OP_CONTEXT,
      [OP_LAYER] = &&DO_OP_LAYER,
      [OP_ACTIVATE] = &&DO_OP_ACTIVATE,
      [OP_END_ACTIVATE] = &&DO_OP_END_ACTIVATE
  };
  #pragma GCC diagnostic pop

  DISPATCH();
  #define CASE_OP(name) DO_##name:
#else
  #define DISPATCH() break
  #define CASE_OP(name) case name:
  for (;;) {
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
#endif

  CASE_OP(OP_CONSTANT) {
      PUSH(READ_CONSTANT());
      DISPATCH();
  }
  
  CASE_OP(OP_NOP) {
      DISPATCH();
  }
  
  CASE_OP(OP_NIL) {
      PUSH(NULL_VAL);
      DISPATCH();
  }
  
  CASE_OP(OP_TRUE) {
      PUSH(BOOL_VAL(true));
      DISPATCH();
  }
  
  CASE_OP(OP_FALSE) {
      PUSH(BOOL_VAL(false));
      DISPATCH();
  }
  
  CASE_OP(OP_POP) {
      pop(vm);
      DISPATCH();
  }
  
  CASE_OP(OP_DUP) {
      PUSH(peek(vm, 0));
      DISPATCH();
  }
  
  CASE_OP(OP_BUILD_LIST) {
      int count = READ_BYTE();
      ObjList* list = newList();
      PUSH(OBJ_VAL(list)); 
      if (count > 0) {
          list->items = ALLOCATE(Value, count);
          list->capacity = count;
          list->count = count;
          for (int i = count - 1; i >= 0; i--) {
              list->items[i] = peek(vm, 1 + (count - 1 - i));
          }
          Value listVal = peek(vm, 0);
          if (vm->stackTop - (count + 1) < vm->stack) {
              runtimeError(vm, "Stack underflow building list.");
              return INTERPRET_RUNTIME_ERROR;
          }
          vm->stackTop -= (count + 1);
          PUSH(listVal);
      }
      DISPATCH();
  }
  
  CASE_OP(OP_BUILD_MAP) {
      int count = READ_BYTE();
      ObjDictionary* dict = newDictionary();
      PUSH(OBJ_VAL(dict)); 
      for (int i = 0; i < count; i++) {
          Value val = peek(vm, 1);
          Value key = peek(vm, 2);
          if (!IS_STRING(key)) {
              runtimeError(vm, "Dictionary key must be a string.");
              return INTERPRET_RUNTIME_ERROR;
          }
          tableSet(&dict->items, AS_STRING(key), val);
          Value dictVal = pop(vm);
          pop(vm); pop(vm); 
          PUSH(dictVal);
      }
      DISPATCH();
  }
  
  CASE_OP(OP_GET_INDEX) {
      Value indexVal = pop(vm);
      Value targetVal = pop(vm);
      if (IS_LIST(targetVal)) {
          if (!IS_NUMBER(indexVal)) {
              runtimeError(vm, "List index must be a number.");
              return INTERPRET_RUNTIME_ERROR;
          }
          ObjList* list = AS_LIST(targetVal);
          int index = (int)AS_NUMBER(indexVal);
          if (index < 0 || index >= list->count) {
              runtimeError(vm, "List index out of bounds.");
              return INTERPRET_RUNTIME_ERROR;
          }
          PUSH(list->items[index]);
      } else if (IS_DICTIONARY(targetVal)) {
          if (!IS_STRING(indexVal)) {
              runtimeError(vm, "Dictionary key must be a string.");
              return INTERPRET_RUNTIME_ERROR;
          }
          ObjDictionary* dict = AS_DICTIONARY(targetVal);
          Value val;
          if (tableGet(&dict->items, AS_STRING(indexVal), &val)) {
              PUSH(val);
          } else {
              PUSH(NIL_VAL); 
          }
      } else {
          runtimeError(vm, "Can only index lists and dictionaries.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_SET_INDEX) {
      Value value = peek(vm, 0);
      Value indexVal = peek(vm, 1);
      Value targetVal = peek(vm, 2);
      if (IS_LIST(targetVal)) {
          if (!IS_NUMBER(indexVal)) {
              runtimeError(vm, "List index must be a number.");
              return INTERPRET_RUNTIME_ERROR;
          }
          ObjList* list = AS_LIST(targetVal);
          int index = (int)AS_NUMBER(indexVal);
          if (index < 0 || index >= list->count) {
              runtimeError(vm, "List index out of bounds.");
              return INTERPRET_RUNTIME_ERROR;
          }
          list->items[index] = value;
          pop(vm); pop(vm); pop(vm);
          PUSH(value);
      } else if (IS_DICTIONARY(targetVal)) {
          if (!IS_STRING(indexVal)) {
              runtimeError(vm, "Dictionary key must be a string.");
              return INTERPRET_RUNTIME_ERROR;
          }
          ObjDictionary* dict = AS_DICTIONARY(targetVal);
          tableSet(&dict->items, AS_STRING(indexVal), value);
          pop(vm); pop(vm); pop(vm);
          PUSH(value);
      } else {
          runtimeError(vm, "Can only index lists and dictionaries.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_GET_LOCAL) {
      uint8_t slot = READ_BYTE();
      PUSH(frame->slots[slot]);
      DISPATCH();
  }

  CASE_OP(OP_GET_LOCAL_0) { PUSH(frame->slots[0]); DISPATCH(); }
  CASE_OP(OP_GET_LOCAL_1) { PUSH(frame->slots[1]); DISPATCH(); }
  CASE_OP(OP_GET_LOCAL_2) { PUSH(frame->slots[2]); DISPATCH(); }
  CASE_OP(OP_GET_LOCAL_3) { PUSH(frame->slots[3]); DISPATCH(); }

  CASE_OP(OP_SET_LOCAL) {
      uint8_t slot = READ_BYTE();
      frame->slots[slot] = peek(vm, 0);
      DISPATCH();
  }

  CASE_OP(OP_SET_LOCAL_0) { frame->slots[0] = peek(vm, 0); DISPATCH(); }
  CASE_OP(OP_SET_LOCAL_1) { frame->slots[1] = peek(vm, 0); DISPATCH(); }
  CASE_OP(OP_SET_LOCAL_2) { frame->slots[2] = peek(vm, 0); DISPATCH(); }
  CASE_OP(OP_SET_LOCAL_3) { frame->slots[3] = peek(vm, 0); DISPATCH(); }

  
  CASE_OP(OP_GET_GLOBAL) {
      ObjString* name = READ_STRING();
      Value value;
      if (vm->activeContextCount > 0) {
          if (resolveContextualMethod(vm, name, &value)) {
              PUSH(value);
              DISPATCH();
          }
      }
      if (!tableGet(&vm->globals, name, &value)) {
        runtimeError(vm, "Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      PUSH(value);
      DISPATCH();
  }
  
  CASE_OP(OP_DEFINE_GLOBAL) {
      ObjString* name = READ_STRING();
      tableSet(&vm->globals, name, peek(vm, 0));
      pop(vm);
      DISPATCH();
  }
  
  CASE_OP(OP_SET_GLOBAL) {
      ObjString* name = READ_STRING();
      Value value;
      if (!tableGet(&vm->globals, name, &value)) {
        runtimeError(vm, "Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      tableSet(&vm->globals, name, peek(vm, 0));
      DISPATCH();
  }
  
  CASE_OP(OP_GET_UPVALUE) {
      uint8_t slot = READ_BYTE();
      PUSH(*frame->closure->upvalues[slot]->location);
      DISPATCH();
  }
  
  CASE_OP(OP_SET_UPVALUE) {
      uint8_t slot = READ_BYTE();
      *frame->closure->upvalues[slot]->location = peek(vm, 0);
      DISPATCH();
  }
  
  CASE_OP(OP_GET_PROPERTY) {
      Value target = peek(vm, 0);
      ObjString* name = READ_STRING();
      if (IS_INSTANCE(target)) {
          ObjInstance* instance = AS_INSTANCE(target);
          Value value;
          if (tableGet(&instance->fields, name, &value)) {
              pop(vm); 
              PUSH(value);
              DISPATCH();
          }
          if (!bindMethod(instance->klass, name, vm)) {
              return INTERPRET_RUNTIME_ERROR;
          }
      } else if (IS_MODULE(target)) {
          ObjModule* module = AS_MODULE(target);
          Value value;
          if (tableGet(&module->exports, name, &value)) {
              pop(vm); 
              PUSH(value);
              DISPATCH();
          }
          runtimeError(vm, "Undefined property '%s' in module '%s'.", name->chars, module->name->chars);
          return INTERPRET_RUNTIME_ERROR;
      } else {
          runtimeError(vm, "Only instances and modules have properties.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_SET_PROPERTY) {
      if (!IS_INSTANCE(peek(vm, 1))) {
        runtimeError(vm, "Only instances have fields.");
        return INTERPRET_RUNTIME_ERROR;
      }
      ObjInstance* instance = AS_INSTANCE(peek(vm, 1));
      tableSet(&instance->fields, READ_STRING(), peek(vm, 0));
      Value value = pop(vm);
      pop(vm);
      PUSH(value);
      DISPATCH();
  }
  
  CASE_OP(OP_GET_SUPER) {
      ObjString* name = READ_STRING();
      ObjClass* superclass = AS_CLASS(pop(vm));
      if (!bindMethod(superclass, name, vm)) {
        return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_EQUAL) {
      Value b = pop(vm);
      Value a = pop(vm);
      if (IS_NUMBER(a) && IS_NUMBER(b)) {
          // IEEE 754 semantics: NaN != NaN
          PUSH(BOOL_VAL(AS_NUMBER(a) == AS_NUMBER(b)));
      } else if (IS_STRING(a) && IS_STRING(b)) {
          ObjString* s1 = AS_STRING(a);
          ObjString* s2 = AS_STRING(b);
          PUSH(BOOL_VAL(s1 == s2 || (s1->length == s2->length && memcmp(s1->chars, s2->chars, s1->length) == 0)));
      } else {
          PUSH(BOOL_VAL(a == b));
      }
      DISPATCH();
  }
  
  CASE_OP(OP_GREATER) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
        runtimeError(vm, "Operands must be numbers.");
        return INTERPRET_RUNTIME_ERROR;
      }
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      PUSH(BOOL_VAL(a > b));
      DISPATCH();
  }
  
  CASE_OP(OP_LESS) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
        runtimeError(vm, "Operands must be numbers.");
        return INTERPRET_RUNTIME_ERROR;
      }
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      PUSH(BOOL_VAL(a < b));
      DISPATCH();
  }
  
  CASE_OP(OP_ADD) {
      if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(pop(vm));
          double a = AS_NUMBER(pop(vm));
          PUSH(NUMBER_VAL(a + b));
      } else if (IS_TENSOR(peek(vm, 0)) || IS_TENSOR(peek(vm, 1))) {
          if (performTensorArithmetic(vm, '+')) { DISPATCH(); }
      } else if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          concatenate(vm);
      } else if (IS_NUMBER(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          Value numVal = peek(vm, 0);
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          PUSH(OBJ_VAL(copyString(buffer, (int)strlen(buffer))));
          vm->stackTop[-2] = vm->stackTop[-1];
          pop(vm);
          concatenate(vm);
      } else if (IS_STRING(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          Value numVal = peek(vm, 1);
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          Value newA = OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
          PUSH(newA);
          vm->stackTop[-3] = vm->stackTop[-1];
          pop(vm);
          concatenate(vm);
      } else {
          runtimeError(vm, "Operands must be numbers or strings.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_SUBTRACT) {
      if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(pop(vm));
          double a = AS_NUMBER(pop(vm));
          PUSH(NUMBER_VAL(a - b));
      } else if (performTensorArithmetic(vm, '-')) { 
          DISPATCH(); 
      } else {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_MULTIPLY) {
      if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(pop(vm));
          double a = AS_NUMBER(pop(vm));
          PUSH(NUMBER_VAL(a * b));
      } else if (performTensorArithmetic(vm, '*')) { 
          DISPATCH(); 
      } else {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_DIVIDE) {
      if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(pop(vm));
          if (b == 0) {
              runtimeError(vm, "Division by zero.");
              return INTERPRET_RUNTIME_ERROR;
          }
          double a = AS_NUMBER(pop(vm));
          PUSH(NUMBER_VAL(a / b));
      } else if (performTensorArithmetic(vm, '/')) { 
          DISPATCH(); 
      } else {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  
  CASE_OP(OP_NOT) {
      PUSH(BOOL_VAL(isFalsey(pop(vm))));
      DISPATCH();
  }
  
  CASE_OP(OP_NEGATE) {
      if (!IS_NUMBER(peek(vm, 0))) {
        runtimeError(vm, "Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      PUSH(NUMBER_VAL(-AS_NUMBER(pop(vm))));
      DISPATCH();
  }
  
  CASE_OP(OP_PRINT) {
      printValue(pop(vm));
      printf("\n");
      fflush(stdout);
      DISPATCH();
  }
  
  CASE_OP(OP_JUMP) {
      uint16_t offset = READ_SHORT();
      frame->ip += offset;
      DISPATCH();
  }
  
  CASE_OP(OP_JUMP_IF_FALSE) {
      uint16_t offset = READ_SHORT();
      if (isFalsey(peek(vm, 0))) frame->ip += offset;
      DISPATCH();
  }
  
  CASE_OP(OP_LOOP) {
      uint16_t offset = READ_SHORT();
      frame->ip -= offset;
      DISPATCH();
  }
  
  CASE_OP(OP_CALL) {
      int argCount = READ_BYTE();
      Value callee = peek(vm, argCount);
      if (IS_CLOSURE(callee)) {
          ObjClosure* closure = AS_CLOSURE(callee);
          if (argCount != closure->function->arity) {
              runtimeError(vm, "Expected %d arguments but got %d.", closure->function->arity, argCount);
              return INTERPRET_RUNTIME_ERROR;
          }
          if (vm->frameCount == FRAMES_MAX) {
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
          PUSH(result);
          DISPATCH();
      } else if (IS_FOREIGN(callee)) {
          ObjForeign* foreign = AS_FOREIGN(callee);
          Value result = callForeign(foreign, argCount, vm->stackTop - argCount);
          vm->stackTop -= argCount + 1;
          PUSH(result);
          DISPATCH();
      } else if (IS_CLASS(callee)) {
          if (!callValue(callee, argCount, vm)) {
              return INTERPRET_RUNTIME_ERROR;
          }
          frame = &vm->frames[vm->frameCount - 1]; 
          DISPATCH();
      } else if (IS_BOUND_METHOD(callee)) {
          ObjBoundMethod* bound = AS_BOUND_METHOD(callee);
          vm->stackTop[-argCount - 1] = bound->receiver;
          if (vm->frameCount == FRAMES_MAX) {
               runtimeError(vm, "Stack overflow.");
               return INTERPRET_RUNTIME_ERROR;
          }
          frame = &vm->frames[vm->frameCount++];
          frame->closure = bound->method;
          frame->ip = bound->method->function->chunk.code;
          frame->slots = vm->stackTop - argCount - 1;
          DISPATCH();
      } else {
          runtimeError(vm, "Can only call functions, classes, and foreign functions.");
          return INTERPRET_RUNTIME_ERROR;
      }
  }
  
  CASE_OP(OP_INVOKE) {
      ObjString* method = READ_STRING();
      int argCount = READ_BYTE();
      if (!invoke(method, argCount, vm)) {
          return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
  }
  
  CASE_OP(OP_SUPER_INVOKE) {
      ObjString* method = READ_STRING();
      int argCount = READ_BYTE();
      ObjClass* superclass = AS_CLASS(pop(vm));
      if (!invokeFromClass(superclass, method, argCount, vm)) {
          return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
  }
  
  CASE_OP(OP_CLOSURE) {
      ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
      ObjClosure* closure = newClosure(function);
      PUSH(OBJ_VAL(closure));
      for (int i = 0; i < closure->upvalueCount; i++) {
          uint8_t isLocal = READ_BYTE();
          uint8_t index = READ_BYTE();
          if (isLocal) {
              closure->upvalues[i] = captureUpvalue(frame->slots + index, vm);
          } else {
              closure->upvalues[i] = frame->closure->upvalues[index];
          }
      }
      DISPATCH();
  }
  
  CASE_OP(OP_CLOSE_UPVALUE) {
      closeUpvalues(vm, vm->stackTop - 1);
      pop(vm);
      DISPATCH();
  }
  
  CASE_OP(OP_RETURN) {
      Value result = pop(vm);
      vm->frameCount--;
      if (vm->frameCount == 0) {
        pop(vm);
        return INTERPRET_OK;
      }
      vm->stackTop = frame->slots;
      PUSH(result);
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
  }
  
  CASE_OP(OP_CLASS) {
      PUSH(OBJ_VAL(newClass(READ_STRING())));
      DISPATCH();
  }
  
  CASE_OP(OP_INHERIT) {
      Value superclass = peek(vm, 1);
      if (!IS_CLASS(superclass)) {
        runtimeError(vm, "Superclass must be a class.");
        return INTERPRET_RUNTIME_ERROR;
      }
      ObjClass* subclass = AS_CLASS(peek(vm, 0));
      tableAddAll(&AS_CLASS(superclass)->methods, &subclass->methods);
      pop(vm); // Pop subclass, keep superclass
      DISPATCH();
  }
  
  CASE_OP(OP_METHOD) {
      defineMethod(READ_STRING(), vm);
      DISPATCH();
  }
  
  CASE_OP(OP_USE) {
      ObjString* name = READ_STRING();
      Value moduleVal;
      if (!tableGet(&vm->importer.modules, name, &moduleVal)) {
          runtimeError(vm, "Could not find module '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
      }
      PUSH(moduleVal);
      DISPATCH();
  }
  
  CASE_OP(OP_INTERFACE) {
      ObjString* name = AS_STRING(READ_CONSTANT());
      PUSH(OBJ_VAL(newInterface(name)));
      DISPATCH();
  }
  
  CASE_OP(OP_IMPLEMENT) {
      Value interfaceVal = pop(vm);
      Value classVal = peek(vm, 0);
      if (!IS_INTERFACE(interfaceVal) || !IS_CLASS(classVal)) {
          runtimeError(vm, "Expected interface and class.");
          return INTERPRET_RUNTIME_ERROR;
      }
      ObjClass* klass = AS_CLASS(classVal);
      klass->interfaces = GROW_ARRAY(Value, klass->interfaces, klass->interfaceCount, klass->interfaceCount + 1);
      klass->interfaces[klass->interfaceCount++] = interfaceVal;
      DISPATCH();
  }
  
  CASE_OP(OP_TRY)
  CASE_OP(OP_CATCH)
  CASE_OP(OP_END_TRY) {
      runtimeError(vm, "Exception handling not yet implemented.");
      return INTERPRET_RUNTIME_ERROR;
  }
  
  CASE_OP(OP_MAKE_FOREIGN) {
      if (!IS_STRING(peek(vm, 0)) || !IS_STRING(peek(vm, 1))) {
          runtimeError(vm, "Foreign symbol and library name must be strings.");
          return INTERPRET_RUNTIME_ERROR;
      }
      ObjString* symbol = AS_STRING(pop(vm));
      ObjString* libName = AS_STRING(pop(vm));
      ObjForeign* foreign = loadForeign(libName, symbol);
      if (foreign == NULL) {
          runtimeError(vm, "Failed to load foreign symbol '%s' from '%s'.", symbol->chars, libName->chars);
          return INTERPRET_RUNTIME_ERROR;
      }
      PUSH(OBJ_VAL(foreign));
      DISPATCH();
  }
  
  CASE_OP(OP_MODULO) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
        runtimeError(vm, "Operands must be numbers.");
        return INTERPRET_RUNTIME_ERROR;
      }
      double b = AS_NUMBER(pop(vm));
      if (b == 0) {
          runtimeError(vm, "Modulo by zero.");
          return INTERPRET_RUNTIME_ERROR;
      }
      double a = AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL(fmod(a, b)));
      DISPATCH();
  }
  
  CASE_OP(OP_BIT_AND) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)(a & b)));
      DISPATCH();
  }
  
  CASE_OP(OP_BIT_OR) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)(a | b)));
      DISPATCH();
  }
  
  CASE_OP(OP_BIT_XOR) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)(a ^ b)));
      DISPATCH();
  }
  
  CASE_OP(OP_BIT_NOT) {
      if (!IS_NUMBER(peek(vm, 0))) {
          runtimeError(vm, "Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)(~a)));
      DISPATCH();
  }
  
  CASE_OP(OP_LEFT_SHIFT) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int b = (int)AS_NUMBER(pop(vm));
      if (b < 0 || b >= 32) {
          runtimeError(vm, "Shift amount must be between 0 and 31.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)((uint32_t)a << b)));
      DISPATCH();
  }
  
  CASE_OP(OP_RIGHT_SHIFT) {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int b = (int)AS_NUMBER(pop(vm));
      if (b < 0 || b >= 32) {
          runtimeError(vm, "Shift amount must be between 0 and 31.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int a = (int)AS_NUMBER(pop(vm));
      PUSH(NUMBER_VAL((double)((uint32_t)a >> b)));
      DISPATCH();
  }
  
  CASE_OP(OP_MAT_MUL) {
      Value bVal = peek(vm, 0);
      Value aVal = peek(vm, 1);
      if (!IS_TENSOR(aVal) || !IS_TENSOR(bVal)) {
          runtimeError(vm, "Operands for '@' must be Tensors.");
          return INTERPRET_RUNTIME_ERROR;
      }
      ObjTensor *a = AS_TENSOR(aVal);
      ObjTensor *b = AS_TENSOR(bVal);
      if (a->dimCount == 1 && b->dimCount == 1) {
          if (a->dims[0] != b->dims[0]) {
              runtimeError(vm, "Vector length mismatch.");
              return INTERPRET_RUNTIME_ERROR;
          }
          double dot = 0;
          for (int i = 0; i < a->dims[0]; i++) dot += a->data[i] * b->data[i];
          pop(vm); pop(vm);
          PUSH(NUMBER_VAL(dot));
          DISPATCH();
      }
      if (a->dimCount != 2 || b->dimCount != 2 || a->dims[1] != b->dims[0]) {
          runtimeError(vm, "Incompatible tensor dimensions for '@'.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int outDims[] = {a->dims[0], b->dims[1]};
      ObjTensor *res = newTensor(2, outDims, NULL);
      PUSH(OBJ_VAL(res));
      for (int i = 0; i < a->dims[0]; i++) {
          for (int k = 0; k < a->dims[1]; k++) {
              double val_a = a->data[i * a->dims[1] + k];
              for (int j = 0; j < b->dims[1]; j++) {
                  res->data[i * b->dims[1] + j] += val_a * b->data[k * b->dims[1] + j];
              }
          }
      }
      Value resVal = pop(vm); pop(vm); pop(vm);
      PUSH(resVal);
      DISPATCH();
  }
  
  CASE_OP(OP_MAKE_TENSOR) {
      int dimCount = READ_BYTE();
      // dimCount is uint8_t, so it's always <= 255. No extra check needed.
      uint32_t elementCount = 0;
      elementCount |= ((uint32_t)*frame->ip++);
      elementCount |= ((uint32_t)*frame->ip++ << 8);
      elementCount |= ((uint32_t)*frame->ip++ << 16);
      elementCount |= ((uint32_t)*frame->ip++ << 24);
      int dims[256]; // Increased from 16 to handle max dimCount
      int totalSize = 1;
      for (int i = 0; i < dimCount; i++) {
           uint32_t d = 0;
           d |= ((uint32_t)*frame->ip++); 
           d |= ((uint32_t)*frame->ip++ << 8);
           d |= ((uint32_t)*frame->ip++ << 16); 
           d |= ((uint32_t)*frame->ip++ << 24);
           dims[i] = (int)d;
           
           // Safety check for dimension size and total size to prevent overflow/OOM
           if (dims[i] < 0 || dims[i] > 1000000) {
               runtimeError(vm, "Tensor dimension too large.");
               return INTERPRET_RUNTIME_ERROR;
           }
           
           long long newSize = (long long)totalSize * dims[i];
           if (newSize > 100000000) { // Max 100M elements
               runtimeError(vm, "Tensor total size exceeds limit.");
               return INTERPRET_RUNTIME_ERROR;
           }
           totalSize = (int)newSize;
      }
      ObjTensor *tensor = newTensor(dimCount, dims, NULL);
      PUSH(OBJ_VAL(tensor)); 
      if (elementCount == (uint32_t)totalSize) {
          if (vm->stackTop - totalSize < vm->stack) {
              runtimeError(vm, "Stack underflow building tensor.");
              return INTERPRET_RUNTIME_ERROR;
          }
          for (int i = totalSize - 1; i >= 0; i--) {
              Value val = peek(vm, 1 + (totalSize - 1 - i));
              tensor->data[i] = IS_NUMBER(val) ? AS_NUMBER(val) : 0;
          }
          Value res = pop(vm);
          vm->stackTop -= totalSize;
          PUSH(res);
      } else {
          memset(tensor->data, 0, totalSize * sizeof(double));
      }
      DISPATCH();
  }
  
  CASE_OP(OP_CONTEXT) {
      PUSH(OBJ_VAL(newContext(READ_STRING())));
      DISPATCH();
  }
  
  CASE_OP(OP_LAYER) {
      ObjString* name = READ_STRING();
      ObjLayer* layer = newLayer(name);
      PUSH(OBJ_VAL(layer));
      Value contextVal = peek(vm, 1);
      if (IS_CONTEXT(contextVal)) {
          tableSet(&AS_CONTEXT(contextVal)->layers, name, OBJ_VAL(layer));
      }
      DISPATCH();
  }
  
  CASE_OP(OP_ACTIVATE) {
      Value contextVal = pop(vm);
      if (!IS_CONTEXT(contextVal)) {
          runtimeError(vm, "Can only activate contexts.");
          return INTERPRET_RUNTIME_ERROR;
      }
      if (vm->activeContextCount >= 64) {
          runtimeError(vm, "Context overflow.");
          return INTERPRET_RUNTIME_ERROR;
      }
      vm->activeContextStack[vm->activeContextCount++] = AS_CONTEXT(contextVal);
      DISPATCH();
  }
  
  CASE_OP(OP_END_ACTIVATE) {
      if (vm->activeContextCount > 0) vm->activeContextCount--;
      DISPATCH();
  }

  {
      runtimeError(pvm, "Unknown opcode %d.", frame->ip[-1]);
      return INTERPRET_RUNTIME_ERROR;
  }

  // End of opcodes
#ifndef __GNUC__
    default:
      runtimeError(pvm, "Unknown opcode %d.", instruction);
      return INTERPRET_RUNTIME_ERROR;
    }
  }
#endif
#undef vm

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef PUSH
#undef DISPATCH
#undef CASE_OP
}

InterpretResult interpretAST(VM* pvm, StmtList* statements) {
  // Disable GC during compilation to prevent freeing unrooted function/constants
  size_t oldNextGC = pvm->nextGC;
  pvm->nextGC = (size_t)-1; // SIZE_MAX

  ObjFunction* function = newFunction();
  
  // Connect the AST-based bytecode generator
  if (!generateBytecode(statements, function)) {
      pvm->nextGC = oldNextGC;
      return INTERPRET_COMPILE_ERROR;
  }
  
  // printf("DEBUG: Generated bytecode. Function: %p\n", function);
  if (function->chunk.code == NULL) {
      fprintf(stderr, "Fatal Error: Bytecode generation produced NULL chunk code.\n");
      pvm->nextGC = oldNextGC;
      return INTERPRET_COMPILE_ERROR;
  } else {
      // printf("DEBUG: Chunk code size: %d\n", function->chunk.count);
  }

  
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

  // printf("DEBUG: Starting execution...\n");
  InterpretResult result = run(pvm);
  // printf("DEBUG: Execution finished with result: %d\n", result);

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

void defineNative(VM* pvm, const char* name, NativeFn function) {
  push(pvm, OBJ_VAL(copyString(name, (int)strlen(name))));
  push(pvm, OBJ_VAL(newNative(function)));
  tableSet(&pvm->globals, AS_STRING(pvm->stackTop[-2]), pvm->stackTop[-1]);
  pop(pvm);
  pop(pvm);
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
    
    // Disable GC during raw chunk execution to prevent freeing transient objects
    size_t oldNextGC = pvm->nextGC;
    pvm->nextGC = (size_t)-1;
    
    InterpretResult result = run(pvm);
    
    // Restore GC
    pvm->nextGC = oldNextGC;
    
    // CRITICAL: Prevent double-free of chunk code/constants.
    // The ObjFunction 'function' now points to the same memory as the input 'chunk'.
    // When 'function' is eventually GC'd, it would try to free this memory.
    // By zeroing it out here, we ensure 'function' doesn't own the buffers.
    // This is safe because 'run' has finished and the closure/function are 
    // about to be unreachable or are owned by the caller's stack management.
    initChunk(&function->chunk);
    
    return result;
}

