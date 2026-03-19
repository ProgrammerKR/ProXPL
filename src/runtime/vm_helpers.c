// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/debug.h"
#include "../include/compiler.h"


void closeUpvalues(VM *pVM, Value *last) {
  while (pVM->openUpvalues != NULL &&
         pVM->openUpvalues->location >= last) {
    ObjUpvalue *upvalue = pVM->openUpvalues;
    upvalue->closed = *upvalue->location;
    upvalue->location = &upvalue->closed;
    pVM->openUpvalues = upvalue->next;
  }
}

ObjUpvalue *captureUpvalue(Value *local, VM *pVM) {
  ObjUpvalue *prevUpvalue = NULL;
  ObjUpvalue *upvalue = pVM->openUpvalues;
  while (upvalue != NULL && upvalue->location > local) {
    prevUpvalue = upvalue;
    upvalue = upvalue->next;
  }

  if (upvalue != NULL && upvalue->location == local) {
    return upvalue;
  }

  ObjUpvalue *createdUpvalue = newUpvalue(local);
  createdUpvalue->next = upvalue;

  if (prevUpvalue == NULL) {
    pVM->openUpvalues = createdUpvalue;
  } else {
    prevUpvalue->next = createdUpvalue;
  }

  return createdUpvalue;
}

void defineMethod(ObjString *name, VM *pVM) {
  Value method = peek(pVM, 0);
  ObjClass *klass = AS_CLASS(peek(pVM, 1));
  if (IS_CLOSURE(method)) {
      AS_CLOSURE(method)->function->ownerClass = klass;
  }
  tableSet(&klass->methods, name, method);
  pop(pVM);
}

bool bindMethod(struct ObjClass *klass, ObjString *name, VM *pVM) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError(pVM, "Undefined property '%s'.", name->chars);
    return false;
  }

  ObjBoundMethod *bound = newBoundMethod(peek(pVM, 0), AS_CLOSURE(method));
  pop(pVM);
  push(pVM, OBJ_VAL(bound));
  return true;
}

bool call(ObjClosure *closure, int argCount, VM *pVM) {
  if (argCount != closure->function->arity) {
    runtimeError(pVM, "Expected %d arguments but got %d.",
                 closure->function->arity, argCount);
    return false;
  }

  if (pVM->frameCount == FRAMES_MAX) {
    runtimeError(pVM, "Stack overflow.");
    return false;
  }

  CallFrame *frame = &pVM->frames[pVM->frameCount++];
  frame->closure = closure;
  frame->ip = closure->function->chunk.code;
  frame->slots = pVM->stackTop - argCount - 1;
  return true;
}

bool callValue(Value callee, int argCount, VM *pVM) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
    case OBJ_BOUND_METHOD: {
      ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
      pVM->stackTop[-argCount - 1] = bound->receiver;
      return call(bound->method, argCount, pVM);
    }
    case OBJ_CLASS: {
      ObjClass *klass = AS_CLASS(callee);
      pVM->stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));
      Value initializer;
      if (tableGet(&klass->methods, pVM->initString, &initializer)) {
        return call(AS_CLOSURE(initializer), argCount, pVM);
      } else if (argCount != 0) {
        runtimeError(pVM, "Expected 0 arguments but got %d.", argCount);
        return false;
      }
      return true;
    }
    case OBJ_CLOSURE:
      return call(AS_CLOSURE(callee), argCount, pVM);
    case OBJ_NATIVE: {
      NativeFn native = AS_NATIVE(callee);
      Value result = native(argCount, pVM->stackTop - argCount);
      pVM->stackTop -= argCount + 1;
      push(pVM, result);
      return true;
    }
    default:
      break; // Non-callable object type
    }
  }
  runtimeError(pVM, "Can only call functions and classes.");
  return false;
}

bool invokeFromClass(struct ObjClass *klass, ObjString *name,
                            int argCount, VM *pVM) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError(pVM, "Undefined property '%s'.", name->chars);
    return false;
  }
  return call(AS_CLOSURE(method), argCount, pVM);
}

bool invoke(ObjString *name, int argCount, VM *pVM) {
  Value receiver = peek(pVM, argCount);

  if (!IS_INSTANCE(receiver)) {
    runtimeError(pVM, "Only instances have methods.");
    return false;
  }

  struct ObjInstance *instance = AS_INSTANCE(receiver);

  Value value;
  if (tableGet(&instance->fields, name, &value)) {
    pVM->stackTop[-argCount - 1] = value;
    return callValue(value, argCount, pVM);
  }

  return invokeFromClass(instance->klass, name, argCount, pVM);
}

void appendToList(ObjList* list, Value value) {
    if (list->capacity < list->count + 1) {
        int oldCapacity = list->capacity;
        list->capacity = GROW_CAPACITY(oldCapacity);
        list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
    }
    list->items[list->count++] = value;
}
