// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

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
  freeObjects(pvm);
}

// Runtime Error Helper
void runtimeError(VM* pvm, const char* format, ...) {
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
  if (pvm->stackTop >= pvm->stack + STACK_MAX) {
      // Emergency stack reset or hard abort to prevent segfault
      // Since we can't easily return error code from push, we set a flag or just abort.
      // Ideally, push checks should be done before calling logic. 
      // check frame overflow handles recursion. 
      // check value stack overflow:
      fprintf(stderr, "Fatal Runtime Error: Value stack overflow.\n");
      exit(1); 
  }
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
            if (entry->key == NULL || IS_BOOL(entry->value)) continue; // Skip empty/tombstone
            
            ObjLayer* layer = AS_LAYER(entry->value);
            if (tableGet(&layer->methods, name, result)) {
                return true;
            }
        }
    }
    return false;
}

// Helper functions moved to vm_helpers.c to avoid duplication


static InterpretResult run(VM* vm) {
  // printf("DEBUG: Entering run()\n");
  CallFrame* frame = &vm->frames[vm->frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

#ifdef __GNUC__
  #define DISPATCH() goto *dispatch_table[*frame->ip++]
  
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverride-init"
  static void* dispatch_table[256] = {
      [0 ... 255] = &&DO_OP_UNKNOWN,
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
      [OP_SET_LOCAL] = &&DO_OP_SET_LOCAL,
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

  DO_OP_CONSTANT: {
      push(vm, READ_CONSTANT());
      DISPATCH();
  }
  DO_OP_NOP: {
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
  DO_OP_DUP: {
      push(vm, peek(vm, 0));
      DISPATCH();
  }
  DO_OP_BUILD_LIST: {
      int count = READ_BYTE();
      struct ObjList* list = newList();
      push(vm, OBJ_VAL(list)); // Protect list from GC
      if (count > 0) {
          list->items = ALLOCATE(Value, count);
          list->capacity = count;
          list->count = count;
          for (int i = count - 1; i >= 0; i--) {
              // Items are on stack. pop() is safe here because list->items is ALREADY allocated and list is rooted.
              // Wait, if we pop, we store into list. list->items[i] = pop(vm);
              // Allocation happened above. Logic is fine.
              // Stack: [item1, item2, ..., itemN, list]
              // We need to pop items BELOW list.
              // pop(vm) returns top (list).
              // We need to fill list items from stack below list.
              // Correct logic using peek/copy then clean stack:
              list->items[i] = peek(vm, 1 + (count - 1 - i));
          }
          // Now clear the stack items
          // Stack top is list. Below are count items.
          // vm->stackTop points to list+1
          // We want to keep list, remove count items.
          *(vm->stackTop - 1 - count) = *(vm->stackTop - 1); // Move list down
          vm->stackTop -= count;
      }
      // list is on top
      DISPATCH();
  }
  DO_OP_BUILD_MAP: {
      int count = READ_BYTE();
      struct ObjDictionary* dict = newDictionary();
      push(vm, OBJ_VAL(dict)); // Protect dict from GC
      for (int i = 0; i < count; i++) {
          Value val = peek(vm, 1);
          Value key = peek(vm, 2);
          if (!IS_STRING(key)) {
               runtimeError(vm, "Dictionary key must be a string.");
               return INTERPRET_RUNTIME_ERROR;
          }
          tableSet(&dict->items, AS_STRING(key), val);
          pop(vm); // dict
          pop(vm); // val
          pop(vm); // key
          push(vm, OBJ_VAL(dict)); // Repush dict
      }
      // dict is already on stack top
      DISPATCH();
  }
  DO_OP_GET_INDEX: {
      Value indexVal = pop(vm);
      Value targetVal = pop(vm);
      if (IS_LIST(targetVal)) {
          if (!IS_NUMBER(indexVal)) {
              runtimeError(vm, "List index must be a number.");
              return INTERPRET_RUNTIME_ERROR;
          }
          struct ObjList* list = AS_LIST(targetVal);
          int index = (int)AS_NUMBER(indexVal);
          if (index < 0 || index >= list->count) {
              runtimeError(vm, "List index out of bounds.");
              return INTERPRET_RUNTIME_ERROR;
          }
          push(vm, list->items[index]);
      } else if (IS_DICTIONARY(targetVal)) {
          if (!IS_STRING(indexVal)) {
              runtimeError(vm, "Dictionary key must be a string.");
              return INTERPRET_RUNTIME_ERROR;
          }
          struct ObjDictionary* dict = AS_DICTIONARY(targetVal);
          Value val;
          if (tableGet(&dict->items, AS_STRING(indexVal), &val)) {
              push(vm, val);
          } else {
              push(vm, NIL_VAL); 
          }
      } else {
          runtimeError(vm, "Can only index lists and dictionaries.");
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_SET_INDEX: {
      Value value = peek(vm, 0); // Keep on stack to protect from GC
      Value indexVal = peek(vm, 1);
      Value targetVal = peek(vm, 2);
      if (IS_LIST(targetVal)) {
          if (!IS_NUMBER(indexVal)) {
              runtimeError(vm, "List index must be a number.");
              return INTERPRET_RUNTIME_ERROR;
          }
          struct ObjList* list = AS_LIST(targetVal);
          int index = (int)AS_NUMBER(indexVal);
          if (index < 0 || index >= list->count) {
              runtimeError(vm, "List index out of bounds.");
              return INTERPRET_RUNTIME_ERROR;
          }
          list->items[index] = value;
          // Clean stack: [target, index, value] -> [value]
          pop(vm); // value
          pop(vm); // index
          pop(vm); // target
          push(vm, value);
      } else if (IS_DICTIONARY(targetVal)) {
          if (!IS_STRING(indexVal)) {
              runtimeError(vm, "Dictionary key must be a string.");
              return INTERPRET_RUNTIME_ERROR;
          }
          struct ObjDictionary* dict = AS_DICTIONARY(targetVal);
          tableSet(&dict->items, AS_STRING(indexVal), value);
          // Clean stack
          pop(vm);
          pop(vm);
          pop(vm);
          push(vm, value);
      } else {
          runtimeError(vm, "Can only index lists and dictionaries.");
          return INTERPRET_RUNTIME_ERROR;
      }
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

      // COP Contextual Dispatch
      if (vm->activeContextCount > 0) {
          if (resolveContextualMethod(vm, name, &value)) {
              push(vm, value);
              DISPATCH();
          }
      }

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
  DO_OP_GET_UPVALUE: {
      uint8_t slot = READ_BYTE();
      push(vm, *frame->closure->upvalues[slot]->location);
      DISPATCH();
  }
  DO_OP_SET_UPVALUE: {
      uint8_t slot = READ_BYTE();
      *frame->closure->upvalues[slot]->location = peek(vm, 0);
      DISPATCH();
  }
  DO_OP_GET_PROPERTY: {
      Value target = peek(vm, 0);
      ObjString* name = READ_STRING();

      #ifdef DEBUG_PROPERTY_ACCESS
      printf("[DEBUG_PROP] OP_GET_PROPERTY: property=%s\n", name->chars);
      printf("[DEBUG_PROP] target: IS_INST=%d IS_MOD=%d\n", 
             IS_INSTANCE(target), IS_MODULE(target));
      #endif

      if (IS_INSTANCE(target)) {
          struct ObjInstance* instance = AS_INSTANCE(target);
          Value value;
          if (tableGet(&instance->fields, name, &value)) {
              #ifdef DEBUG_PROPERTY_ACCESS
              printf("[DEBUG_PROP] Found field '%s': IS_OBJ=%d IS_STRING=%d\n", 
                     name->chars, IS_OBJ(value), IS_STRING(value));
              if (IS_OBJ(value)) {
                  Obj* obj = AS_OBJ(value);
                  printf("[DEBUG_PROP] Field obj: type=%d ptr=%p\n", obj->type, (void*)obj);
                  if (IS_STRING(value)) {
                      ObjString* str = AS_STRING(value);
                      printf("[DEBUG_PROP] Field string: len=%d ptr=%p hash=%u\n",
                             str->length, (void*)str, str->hash);
                  }
              }
              #endif
              pop(vm); // Instance
              push(vm, value);
              DISPATCH();
          }
          if (!bindMethod(instance->klass, name, vm)) {
              return INTERPRET_RUNTIME_ERROR;
          }
      } else if (IS_MODULE(target)) {
          ObjModule* module = AS_MODULE(target);
          Value value;
          if (tableGet(&module->exports, name, &value)) {
              pop(vm); // Module
              push(vm, value);
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
  DO_OP_SET_PROPERTY: {
      if (!IS_INSTANCE(peek(vm, 1))) {
        runtimeError(vm, "Only instances have fields.");
        return INTERPRET_RUNTIME_ERROR;
      }
      struct ObjInstance* instance = AS_INSTANCE(peek(vm, 1));
      tableSet(&instance->fields, READ_STRING(), peek(vm, 0));
      Value value = pop(vm);
      pop(vm);
      push(vm, value);
      DISPATCH();
  }
  DO_OP_GET_SUPER: {
      ObjString* name = READ_STRING();
      struct ObjClass* superclass = AS_CLASS(pop(vm));
      if (!bindMethod(superclass, name, vm)) {
        return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_EQUAL: {
      Value b = pop(vm);
      Value a = pop(vm);
      if (IS_STRING(a) && IS_STRING(b)) {
          ObjString* s1 = AS_STRING(a);
          ObjString* s2 = AS_STRING(b);
          push(vm, BOOL_VAL(s1 == s2 || (s1->length == s2->length && memcmp(s1->chars, s2->chars, s1->length) == 0)));
      } else {
          push(vm, BOOL_VAL(a == b));
      }
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
      } else if (IS_NUMBER(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          // String + Number
          Value numVal = peek(vm, 0); // b
          Value strVal = peek(vm, 1); // a
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          push(vm, OBJ_VAL(copyString(buffer, (int)strlen(buffer))));
          // Stack: [a (str), b (num), new_b (str)]
          // We need to move new_b to b position and pop b, then concatenate.
          // Or just call concatenate differently.
          // concatenate expects [a, b] on top.
          // Current stack: [..., a, b, new_str]
          // Swap b with new_str then pop b.
          vm->stackTop[-2] = vm->stackTop[-1];
          pop(vm);
          concatenate(vm);
      } else if (IS_STRING(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          // Number + String
          Value strVal = peek(vm, 0); // b
          Value numVal = peek(vm, 1); // a
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          Value newStr = OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
          push(vm, newStr);
          // Stack: [..., a (num), b (str), new_a (str)]
          // We want [new_a, b] on top.
          // Current: top-2=a, top-1=b, top=new_a
          vm->stackTop[-3] = vm->stackTop[-1];
          // Stack: [new_a, b, new_a]
          pop(vm);
          concatenate(vm);
      } else {
          // Enhanced Error Logging
          Value v1 = peek(vm, 1); // a
          Value v2 = peek(vm, 0); // b
          runtimeError(vm, "Operands must be two numbers or two strings. Got types %d and %d", 
                      IS_OBJ(v1) ? OBJ_TYPE(v1) : (IS_NUMBER(v1) ? -2 : -1), 
                      IS_OBJ(v2) ? OBJ_TYPE(v2) : (IS_NUMBER(v2) ? -2 : -1));
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_SUBTRACT: {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(a - b));
      DISPATCH();
  }
  DO_OP_MULTIPLY: {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(a * b));
      DISPATCH();
  }
  DO_OP_DIVIDE: {
      if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {
          runtimeError(vm, "Operands must be numbers.");
          return INTERPRET_RUNTIME_ERROR;
      }
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
    fflush(stdout);
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
          if (vm->frameCount == FRAMES_MAX) { // FRAMES_MAX
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

      } else if (IS_FOREIGN(callee)) {
          ObjForeign* foreign = AS_FOREIGN(callee);
          Value result = callForeign(foreign, argCount, vm->stackTop - argCount);
          vm->stackTop -= argCount + 1;
          push(vm, result);
          DISPATCH();
      } else if (IS_CLASS(callee)) {
          if (!callValue(callee, argCount, vm)) {
              return INTERPRET_RUNTIME_ERROR;
          }
          frame = &vm->frames[vm->frameCount - 1]; // Update frame pointer after potential push
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
      DISPATCH();
  }
  DO_OP_INVOKE: {
      ObjString* method = READ_STRING();
      int argCount = READ_BYTE();
      if (!invoke(method, argCount, vm)) {
          return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
  }
  DO_OP_SUPER_INVOKE: {
      ObjString* method = READ_STRING();
      int argCount = READ_BYTE();
      ObjClass* superclass = AS_CLASS(pop(vm));
      if (!invokeFromClass(superclass, method, argCount, vm)) {
          return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm->frames[vm->frameCount - 1];
      DISPATCH();
  }
  DO_OP_CLOSURE: {
      ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
      ObjClosure* closure = newClosure(function);
      push(vm, OBJ_VAL(closure));
      DISPATCH();
  }
  DO_OP_CLOSE_UPVALUE: {
      closeUpvalues(vm, vm->stackTop - 1);
      pop(vm);
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
  DO_OP_CLASS: {
      push(vm, OBJ_VAL(newClass(READ_STRING())));
      DISPATCH();
  }
  DO_OP_INHERIT: {
      Value superclass = peek(vm, 1);
      if (!IS_CLASS(superclass)) {
        runtimeError(vm, "Superclass must be a class.");
        return INTERPRET_RUNTIME_ERROR;
      }
      struct ObjClass* subclass = AS_CLASS(peek(vm, 0));
      tableAddAll(&AS_CLASS(superclass)->methods, &subclass->methods);
      pop(vm); // Subclass
      pop(vm); // Superclass
      DISPATCH();
  }
  DO_OP_METHOD: {
      defineMethod(READ_STRING(), vm);
      DISPATCH();
  }
  DO_OP_USE: {
      ObjString* name = READ_STRING();
      Value moduleVal;
      if (tableGet(&vm->importer.modules, name, &moduleVal)) {
          // Module found.
          // In a full implementation, we might bind it to a variable or ensure it's loaded.
          // For now, finding it validates the 'use'.
          // Pop implicit? use stmt usually doesn't leave value on stack.
          // But our compiler doesn't push a result for USE stmt, it emits OP_USE.
          // Wait, READ_STRING() reads operand. Stack is unchanged.
      } else {
          // Fallback: Try to load file? (Not implemented)
          runtimeError(vm, "Could not find module '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
      }
      DISPATCH();
  }
  DO_OP_TRY:
  DO_OP_CATCH:
  DO_OP_END_TRY: {
      runtimeError(vm, "Exception handling not yet implemented.");
      return INTERPRET_RUNTIME_ERROR;
  }
  DO_OP_MAKE_FOREIGN: {
      ObjString* symbol = AS_STRING(pop(vm));
      ObjString* libName = AS_STRING(pop(vm));
      ObjForeign* foreign = loadForeign(libName, symbol);
      if (foreign == NULL) {
          runtimeError(vm, "Failed to load foreign symbol '%s' from '%s'.", symbol->chars, libName->chars);
          return INTERPRET_RUNTIME_ERROR;
      }
      push(vm, OBJ_VAL(foreign));
      DISPATCH();
  }
  
  DO_OP_MODULO: {
      double b = AS_NUMBER(pop(vm));
      double a = AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL(fmod(a, b)));
      DISPATCH();
  }
  DO_OP_BIT_AND: {
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(a & b)));
      DISPATCH();
  }
  DO_OP_BIT_OR: {
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(a | b)));
      DISPATCH();
  }
  DO_OP_BIT_XOR: {
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(a ^ b)));
      DISPATCH();
  }
  DO_OP_BIT_NOT: {
      if (!IS_NUMBER(peek(vm, 0))) {
          runtimeError(vm, "Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
      }
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(~a)));
      DISPATCH();
  }
  DO_OP_LEFT_SHIFT: {
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(a << b)));
      DISPATCH();
  }
  DO_OP_RIGHT_SHIFT: {
      int b = (int)AS_NUMBER(pop(vm));
      int a = (int)AS_NUMBER(pop(vm));
      push(vm, NUMBER_VAL((double)(a >> b)));
      DISPATCH();
  }
  
  DO_OP_MAT_MUL: {
      Value bVal = peek(vm, 0);
      Value aVal = peek(vm, 1);
      
      if (!IS_TENSOR(aVal) || !IS_TENSOR(bVal)) {
          runtimeError(vm, "Operands for '@' must be Tensors.");
          return INTERPRET_RUNTIME_ERROR;
      }
      
      ObjTensor *a = AS_TENSOR(aVal);
      ObjTensor *b = AS_TENSOR(bVal);

      // Support 1D Dot Product
      if (a->dimCount == 1 && b->dimCount == 1) {
          if (a->dims[0] != b->dims[0]) {
              runtimeError(vm, "Dot product requires vectors of same length: %d and %d.", a->dims[0], b->dims[0]);
              return INTERPRET_RUNTIME_ERROR;
          }
          double dot = 0;
          for (int i = 0; i < a->dims[0]; i++) dot += a->data[i] * b->data[i];
          pop(vm); // b
          pop(vm); // a
          push(vm, NUMBER_VAL(dot));
          DISPATCH();
      }
      
      // Basic 2D Matrix Multiplication logic
      if (a->dimCount != 2 || b->dimCount != 2) {
          runtimeError(vm, "Matrix multiplication currently supports 2D Tensors.");
          return INTERPRET_RUNTIME_ERROR;
      }
      
      int rows_a = a->dims[0];
      int cols_a = a->dims[1];
      int rows_b = b->dims[0];
      int cols_b = b->dims[1];
      
      if (cols_a != rows_b) {
          runtimeError(vm, "Incompatible dimensions for matrix multiplication: %dx%d and %dx%d.", rows_a, cols_a, rows_b, cols_b);
          return INTERPRET_RUNTIME_ERROR;
      }
      
      int outDims[] = {rows_a, cols_b};
      ObjTensor *res = newTensor(2, outDims, NULL);
      push(vm, OBJ_VAL(res)); // Root it
      
      // Multiplication logic (naive ijk for now, compiler can optimize)
      for (int i = 0; i < rows_a; i++) {
          for (int k = 0; k < cols_a; k++) {
              double val_a = a->data[i * cols_a + k];
              for (int j = 0; j < cols_b; j++) {
                  res->data[i * cols_b + j] += val_a * b->data[k * cols_b + j];
              }
          }
      }
      
      Value resVal = pop(vm); // result tensor
      pop(vm); // b
      pop(vm); // a
      push(vm, resVal);
      DISPATCH();
  }
  
  DO_OP_MAKE_TENSOR: {
      int dimCount = READ_BYTE();
      
      // Read element count (4 bytes little endian)
      uint32_t elementCount = 0;
      elementCount |= (*frame->ip++);
      elementCount |= (*frame->ip++ << 8);
      elementCount |= (*frame->ip++ << 16);
      elementCount |= (*frame->ip++ << 24);

      int dims[16];
      int totalSize = 1;
      
      // Read dimensions (4 bytes each)
      for (int i = 0; i < dimCount; i++) {
           uint32_t d = 0;
           d |= (*frame->ip++);
           d |= (*frame->ip++ << 8);
           d |= (*frame->ip++ << 16);
           d |= (*frame->ip++ << 24);
           dims[i] = (int)d;
           totalSize *= dims[i];
      }

      ObjTensor *tensor = newTensor(dimCount, dims, NULL);
      push(vm, OBJ_VAL(tensor)); // Root it

      if (elementCount == 0 && totalSize > 0) {
          // Zero Fill Mode
          memset(tensor->data, 0, totalSize * sizeof(double));
      } else if (elementCount == totalSize) {
          // Pop elements (reverse order)
          for (int i = totalSize - 1; i >= 0; i--) {
              Value val = pop(vm);
              if (IS_NUMBER(val)) {
                  tensor->data[i] = AS_NUMBER(val);
              } else {
                  tensor->data[i] = 0; // Error handling?
              }
          }
      } else {
           // Mismatch or invalid
           // runtimeError(vm, "Tensor size mismatch");
           // For safety, zero fill?
           memset(tensor->data, 0, totalSize * sizeof(double));
      }
      
      DISPATCH();
  }
  

  
  DO_OP_CONTEXT: {
      ObjString* name = READ_STRING();
      push(vm, OBJ_VAL(newContext(name)));
      DISPATCH();
  }

  DO_OP_LAYER: {
      ObjString* name = READ_STRING();
      ObjLayer* layer = newLayer(name);
      push(vm, OBJ_VAL(layer));
      
      Value contextVal = peek(vm, 1);
      if (IS_CONTEXT(contextVal)) {
          tableSet(&AS_CONTEXT(contextVal)->layers, name, OBJ_VAL(layer));
      }
      DISPATCH();
  }

  DO_OP_ACTIVATE: {
      Value contextVal = pop(vm);
      if (!IS_CONTEXT(contextVal)) {
          runtimeError(vm, "Can only activate context objects.");
          return INTERPRET_RUNTIME_ERROR;
      }
      if (vm->activeContextCount >= 64) {
          runtimeError(vm, "Context stack overflow.");
          return INTERPRET_RUNTIME_ERROR;
      }
      vm->activeContextStack[vm->activeContextCount++] = AS_CONTEXT(contextVal);
      DISPATCH();
  }

  DO_OP_END_ACTIVATE: {
      if (vm->activeContextCount > 0) {
          vm->activeContextCount--;
      }
      DISPATCH();
  }

  DO_OP_UNKNOWN: {
      runtimeError(vm, "Unknown opcode %d.", frame->ip[-1]);
      return INTERPRET_RUNTIME_ERROR;
  }
  
#else
  // Fallback for MSVC / Standard C
  for (;;) {
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: push(vm, READ_CONSTANT()); break;
    case OP_NOP: break;
    case OP_NIL: push(vm, NULL_VAL); break;
    case OP_TRUE: push(vm, BOOL_VAL(true)); break;
    case OP_FALSE: push(vm, BOOL_VAL(false)); break;
    case OP_POP: pop(vm); break;
    case OP_DUP: push(vm, peek(vm, 0)); break;
    case OP_BUILD_LIST: {
        int count = READ_BYTE();
        struct ObjList* list = newList();
        push(vm, OBJ_VAL(list)); // Root it!
        if (count > 0) {
            list->items = ALLOCATE(Value, count);
            list->capacity = count;
            list->count = count;
            for (int i = count - 1; i >= 0; i--) {
                // Peek from below the rooted list
                list->items[i] = peek(vm, 1 + (count - 1 - i));
            }
            // Clean stack: [item1, ..., itemN, list] -> [list]
            Value listVal = peek(vm, 0);
            vm->stackTop -= (count + 1);
            push(vm, listVal);
        }
        break;
    }
    case OP_BUILD_MAP: {
        int count = READ_BYTE();
        struct ObjDictionary* dict = newDictionary();
        push(vm, OBJ_VAL(dict)); // Root it!
        for (int i = 0; i < count; i++) {
            // Peek below the rooted dict
            Value val = peek(vm, 1);
            Value key = peek(vm, 2);
            if (!IS_STRING(key)) {
                runtimeError(vm, "Dictionary key must be a string.");
                return INTERPRET_RUNTIME_ERROR;
            }
            tableSet(&dict->items, AS_STRING(key), val);
            
            // Clean up key/val from stack, keep dict
            // Stack: [..., key, val, dict]
            pop(vm); // dict
            pop(vm); // val
            pop(vm); // key
            push(vm, OBJ_VAL(dict));
        }
        break;
    }
    case OP_GET_INDEX: {
        Value indexVal = pop(vm);
        Value targetVal = pop(vm);
        if (IS_LIST(targetVal)) {
            if (!IS_NUMBER(indexVal)) {
                runtimeError(vm, "List index must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            struct ObjList* list = AS_LIST(targetVal);
            int index = (int)AS_NUMBER(indexVal);
            if (index < 0 || index >= list->count) {
                runtimeError(vm, "List index out of bounds.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(vm, list->items[index]);
        } else if (IS_DICTIONARY(targetVal)) {
            if (!IS_STRING(indexVal)) {
                runtimeError(vm, "Dictionary key must be a string.");
                return INTERPRET_RUNTIME_ERROR;
            }
            struct ObjDictionary* dict = AS_DICTIONARY(targetVal);
            Value val;
            if (tableGet(&dict->items, AS_STRING(indexVal), &val)) {
                push(vm, val);
            } else {
                push(vm, NIL_VAL); 
            }
        } else {
            runtimeError(vm, "Can only index lists and dictionaries.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_SET_INDEX: {
        Value value = pop(vm);
        Value indexVal = pop(vm);
        Value targetVal = pop(vm);
        if (IS_LIST(targetVal)) {
            if (!IS_NUMBER(indexVal)) {
                runtimeError(vm, "List index must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            struct ObjList* list = AS_LIST(targetVal);
            int index = (int)AS_NUMBER(indexVal);
            if (index < 0 || index >= list->count) {
                runtimeError(vm, "List index out of bounds.");
                return INTERPRET_RUNTIME_ERROR;
            }
            list->items[index] = value;
            push(vm, value);
        } else if (IS_DICTIONARY(targetVal)) {
            if (!IS_STRING(indexVal)) {
                runtimeError(vm, "Dictionary key must be a string.");
                return INTERPRET_RUNTIME_ERROR;
            }
            struct ObjDictionary* dict = AS_DICTIONARY(targetVal);
            tableSet(&dict->items, AS_STRING(indexVal), value);
            push(vm, value);
        } else {
            runtimeError(vm, "Can only index lists and dictionaries.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
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

        // COP Contextual Dispatch
        if (vm->activeContextCount > 0) {
            if (resolveContextualMethod(vm, name, &value)) {
                push(vm, value);
                break;
            }
        }

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
    case OP_GET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        push(vm, *frame->closure->upvalues[slot]->location);
        break;
    }
    case OP_SET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        *frame->closure->upvalues[slot]->location = peek(vm, 0);
        break;
    }
    case OP_GET_PROPERTY: {
        Value target = peek(vm, 0);
        ObjString* name = READ_STRING();

        if (IS_INSTANCE(target)) {
            ObjInstance* instance = AS_INSTANCE(target);
            Value value;
            if (tableGet(&instance->fields, name, &value)) {
                pop(vm); // Instance
                push(vm, value);
                break;
            }
            if (!bindMethod(instance->klass, name, vm)) {
                return INTERPRET_RUNTIME_ERROR;
            }
        } else if (IS_MODULE(target)) {
            ObjModule* module = AS_MODULE(target);
            Value value;
            if (tableGet(&module->exports, name, &value)) {
                pop(vm); // Module
                push(vm, value);
                break;
            }
            runtimeError(vm, "Undefined property '%s' in module '%s'.", name->chars, module->name->chars);
            return INTERPRET_RUNTIME_ERROR;
        } else {
            runtimeError(vm, "Only instances and modules have properties.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_SET_PROPERTY: {
        if (!IS_INSTANCE(peek(vm, 1))) {
          runtimeError(vm, "Only instances have fields.");
          return INTERPRET_RUNTIME_ERROR;
        }
        ObjInstance* instance = AS_INSTANCE(peek(vm, 1));
        tableSet(&instance->fields, READ_STRING(), peek(vm, 0));
        Value value = pop(vm);
        pop(vm);
        push(vm, value);
        break;
    }
    case OP_GET_SUPER: {
        ObjString* name = READ_STRING();
        ObjClass* superclass = AS_CLASS(pop(vm));
        if (!bindMethod(superclass, name, vm)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_EQUAL: {
        Value b = pop(vm);
        Value a = pop(vm);
        if (IS_STRING(a) && IS_STRING(b)) {
            ObjString* s1 = AS_STRING(a);
            ObjString* s2 = AS_STRING(b);
            push(vm, BOOL_VAL(s1 == s2 || (s1->length == s2->length && memcmp(s1->chars, s2->chars, s1->length) == 0)));
        } else {
            push(vm, BOOL_VAL(a == b));
        }
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
        } else if (IS_NUMBER(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          Value numVal = peek(vm, 0);
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          Value newB = OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
          push(vm, newB);
          vm->stackTop[-2] = vm->stackTop[-1];
          pop(vm);
          concatenate(vm);
        } else if (IS_STRING(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          Value numVal = peek(vm, 1);
          char buffer[32];
          snprintf(buffer, sizeof(buffer), "%.14g", AS_NUMBER(numVal));
          Value newA = OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
          push(vm, newA);
          vm->stackTop[-3] = vm->stackTop[-1];
          pop(vm);
          concatenate(vm);
        } else {
          Value v1 = peek(vm, 1); // a
          Value v2 = peek(vm, 0); // b
          runtimeError(vm, "Operands must be two numbers or two strings. Got types %d and %d", 
                      IS_OBJ(v1) ? OBJ_TYPE(v1) : (IS_NUMBER(v1) ? -2 : -1), 
                      IS_OBJ(v2) ? OBJ_TYPE(v2) : (IS_NUMBER(v2) ? -2 : -1));
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
            if (vm->frameCount == FRAMES_MAX) {
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

        } else if (IS_FOREIGN(callee)) {
            ObjForeign* foreign = AS_FOREIGN(callee);
            Value result = callForeign(foreign, argCount, vm->stackTop - argCount);
            vm->stackTop -= argCount + 1;
            push(vm, result);
            break;
        } else if (IS_CLASS(callee)) {
            if (!callValue(callee, argCount, vm)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm->frames[vm->frameCount - 1];
            break;
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
            break;
        } else {
            runtimeError(vm, "Can only call functions, classes, and foreign functions.");
            return INTERPRET_RUNTIME_ERROR;
        }
    }
    case OP_INVOKE: {
        ObjString* method = READ_STRING();
        int argCount = READ_BYTE();
        if (!invoke(method, argCount, vm)) {
            return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm->frames[vm->frameCount - 1];
        break;
    }
    case OP_SUPER_INVOKE: {
        ObjString* method = READ_STRING();
        int argCount = READ_BYTE();
        ObjClass* superclass = AS_CLASS(pop(vm));
        if (!invokeFromClass(superclass, method, argCount, vm)) {
            return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm->frames[vm->frameCount - 1];
        break;
    }
    case OP_CLOSURE: {
        ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
        ObjClosure* closure = newClosure(function);
        push(vm, OBJ_VAL(closure));
        for (int i = 0; i < closure->upvalueCount; i++) {
            uint8_t isLocal = READ_BYTE();
            uint8_t index = READ_BYTE();
            if (isLocal) {
                closure->upvalues[i] = captureUpvalue(frame->slots + index, vm);
            } else {
                closure->upvalues[i] = frame->closure->upvalues[index];
            }
        }
        break;
    }
    case OP_CLOSE_UPVALUE: {
        closeUpvalues(vm, vm->stackTop - 1);
        pop(vm);
        break;
    }
    case OP_CLASS: {
        push(vm, OBJ_VAL(newClass(READ_STRING())));
        break;
    }
    case OP_INHERIT: {
        Value superclass = peek(vm, 1);
        if (!IS_CLASS(superclass)) {
             runtimeError(vm, "Superclass must be a class.");
             return INTERPRET_RUNTIME_ERROR;
        }
        ObjClass* subclass = AS_CLASS(peek(vm, 0));
        tableAddAll(&AS_CLASS(superclass)->methods, &subclass->methods);
        pop(vm); // Subclass
        pop(vm); // Superclass
        break;
    }
    case OP_METHOD: {
        defineMethod(READ_STRING(), vm);
        break;
    }
    case OP_USE: {
        ObjString* name = READ_STRING();
        Value moduleVal;
        if (tableGet(&vm->importer.modules, name, &moduleVal)) {
            // Module found.
        } else {
            runtimeError(vm, "Could not find module '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_TRY:
    case OP_INTERFACE: {
        ObjString* name = AS_STRING(READ_CONSTANT());
        ObjInterface* interface = newInterface(name);
        push(vm, OBJ_VAL(interface));
        break;
    }
    case OP_IMPLEMENT: {
        Value interfaceVal = pop(vm);
        Value classVal = peek(vm, 0); // Class remains on stack
        
        if (!IS_INTERFACE(interfaceVal)) {
             runtimeError(vm, "Expected interface.");
             return INTERPRET_RUNTIME_ERROR;
        }

        ObjClass* klass = AS_CLASS(classVal);
        int oldCapacity = klass->interfaceCount;
        int newCapacity = oldCapacity + 1;
        klass->interfaces = GROW_ARRAY(Value, klass->interfaces, oldCapacity, newCapacity);
        klass->interfaces[oldCapacity] = interfaceVal;
        klass->interfaceCount = newCapacity;
        
        break;
    }
    case OP_CATCH:
    case OP_END_TRY: {
        runtimeError(vm, "Exception handling not yet implemented.");
        return INTERPRET_RUNTIME_ERROR;
    }
    case OP_MAKE_FOREIGN: {
        ObjString* symbol = AS_STRING(pop(vm));
        ObjString* libName = AS_STRING(pop(vm));
        ObjForeign* foreign = loadForeign(libName, symbol);
        if (foreign == NULL) {
            runtimeError(vm, "Failed to load foreign symbol '%s' from '%s'.", symbol->chars, libName->chars);
           return INTERPRET_RUNTIME_ERROR;
        }
        push(vm, OBJ_VAL(foreign));
        break;
    }
    case OP_MODULO: {
        double b = AS_NUMBER(pop(vm));
        double a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(fmod(a, b)));
        break;
    }
    case OP_BIT_AND: {
        int b = (int)AS_NUMBER(pop(vm));
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(a & b)));
        break;
    }
    case OP_BIT_OR: {
        int b = (int)AS_NUMBER(pop(vm));
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(a | b)));
        break;
    }
    case OP_BIT_XOR: {
        int b = (int)AS_NUMBER(pop(vm));
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(a ^ b)));
        break;
    }
    case OP_BIT_NOT: {
        if (!IS_NUMBER(peek(vm, 0))) {
            runtimeError(vm, "Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
        }
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(~a)));
        break;
    }
    case OP_LEFT_SHIFT: {
        int b = (int)AS_NUMBER(pop(vm));
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(a << b)));
        break;
    }
    case OP_RIGHT_SHIFT: {
        int b = (int)AS_NUMBER(pop(vm));
        int a = (int)AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL((double)(a >> b)));
        break;
    }
    case OP_MAT_MUL: {
        Value bVal = peek(vm, 0);
        Value aVal = peek(vm, 1);
        
        if (!IS_TENSOR(aVal) || !IS_TENSOR(bVal)) {
            runtimeError(vm, "Operands for '@' must be Tensors.");
            return INTERPRET_RUNTIME_ERROR;
        }
        
        ObjTensor *a = AS_TENSOR(aVal);
        ObjTensor *b = AS_TENSOR(bVal);
        
        // Support 1D Dot Product
        if (a->dimCount == 1 && b->dimCount == 1) {
            if (a->dims[0] != b->dims[0]) {
                runtimeError(vm, "Dot product requires vectors of same length: %d and %d.", a->dims[0], b->dims[0]);
                return INTERPRET_RUNTIME_ERROR;
            }
            double dot = 0;
            for (int i = 0; i < a->dims[0]; i++) dot += a->data[i] * b->data[i];
            pop(vm); // b
            pop(vm); // a
            push(vm, NUMBER_VAL(dot));
            break;
        }
        
        if (a->dimCount != 2 || b->dimCount != 2) {
            runtimeError(vm, "Matrix multiplication currently supports 2D Tensors.");
            return INTERPRET_RUNTIME_ERROR;
        }
        
        int rows_a = a->dims[0];
        int cols_a = a->dims[1];
        int rows_b = b->dims[0];
        int cols_b = b->dims[1];
        
        if (cols_a != rows_b) {
            runtimeError(vm, "Incompatible dimensions: %dx%d and %dx%d.", rows_a, cols_a, rows_b, cols_b);
            return INTERPRET_RUNTIME_ERROR;
        }
        
        int outDims[] = {rows_a, cols_b};
        ObjTensor *res = newTensor(2, outDims, NULL);
        push(vm, OBJ_VAL(res));
        
        for (int i = 0; i < rows_a; i++) {
            for (int k = 0; k < cols_a; k++) {
                double val_a = a->data[i * cols_a + k];
                for (int j = 0; j < cols_b; j++) {
                    res->data[i * cols_b + j] += val_a * b->data[k * cols_b + j];
                }
            }
        }
        
        Value resVal = pop(vm);
        pop(vm);
    case OP_MAKE_TENSOR: {
        int dimCount = READ_BYTE();
        
        // Read element count (4 bytes little endian)
        uint32_t elementCount = 0;
        elementCount |= (*frame->ip++);
        elementCount |= (*frame->ip++ << 8);
        elementCount |= (*frame->ip++ << 16);
        elementCount |= (*frame->ip++ << 24);

        int dims[16];
        int totalSize = 1;
        
        // Read dimensions (4 bytes each)
        for (int i = 0; i < dimCount; i++) {
             uint32_t d = 0;
             d |= (*frame->ip++);
             d |= (*frame->ip++ << 8);
             d |= (*frame->ip++ << 16);
             d |= (*frame->ip++ << 24);
             dims[i] = (int)d;
             totalSize *= dims[i];
        }

        ObjTensor *tensor = newTensor(dimCount, dims, NULL);
        push(vm, OBJ_VAL(tensor)); // Root it

        if (elementCount == 0 && totalSize > 0) {
             memset(tensor->data, 0, totalSize * sizeof(double));
        } else if (elementCount == totalSize) {
            // Pop elements (reverse order as they were pushed in order, stack top is last)
            for (int i = totalSize - 1; i >= 0; i--) {
                Value val = pop(vm);
                if (IS_NUMBER(val)) {
                    tensor->data[i] = AS_NUMBER(val);
                } else {
                    tensor->data[i] = 0;
                }
            }
        } else {
             memset(tensor->data, 0, totalSize * sizeof(double));
        }
        break;
    }
    case OP_CONTEXT: {
        ObjString* name = READ_STRING();
        push(vm, OBJ_VAL(newContext(name)));
        break;
    }
    case OP_LAYER: {
        ObjString* name = READ_STRING();
        ObjLayer* layer = newLayer(name);
        push(vm, OBJ_VAL(layer));
        Value contextVal = peek(vm, 1);
        if (IS_CONTEXT(contextVal)) {
            tableSet(&AS_CONTEXT(contextVal)->layers, name, OBJ_VAL(layer));
        }
        break;
    }
    case OP_ACTIVATE: {
        Value contextVal = pop(vm);
        if (!IS_CONTEXT(contextVal)) {
            runtimeError(vm, "Can only activate context objects.");
            return INTERPRET_RUNTIME_ERROR;
        }
        if (vm->activeContextCount >= 64) {
            runtimeError(vm, "Context stack overflow.");
            return INTERPRET_RUNTIME_ERROR;
        }
        vm->activeContextStack[vm->activeContextCount++] = AS_CONTEXT(contextVal);
        break;
    }
    case OP_END_ACTIVATE: {
        if (vm->activeContextCount > 0) vm->activeContextCount--;
        break;
    }
    case OP_RETURN: {
        Value result = pop(vm);
        vm->frameCount--;
        if (vm->frameCount == 0) {
            pop(vm);
            return INTERPRET_OK;
        }
        vm->stackTop = frame->slots;
        push(vm, result);
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

