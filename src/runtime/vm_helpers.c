#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/debug.h"
#include "../include/compiler.h"


void closeUpvalues(VM *vm, Value *last) {
  while (vm->openUpvalues != NULL &&
         vm->openUpvalues->location >= last) {
    ObjUpvalue *upvalue = vm->openUpvalues;
    upvalue->closed = *upvalue->location;
    upvalue->location = &upvalue->closed;
    vm->openUpvalues = upvalue->next;
  }
}

ObjUpvalue *captureUpvalue(Value *local, VM *vm) {
  ObjUpvalue *prevUpvalue = NULL;
  ObjUpvalue *upvalue = vm->openUpvalues;
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
    vm->openUpvalues = createdUpvalue;
  } else {
    prevUpvalue->next = createdUpvalue;
  }

  return createdUpvalue;
}

void defineMethod(ObjString *name, VM *vm) {
  Value method = peek(vm, 0);
  ObjClass *klass = AS_CLASS(peek(vm, 1));
  tableSet(&klass->methods, name, method);
  pop(vm);
}

bool bindMethod(struct ObjClass *klass, ObjString *name, VM *vm) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError(vm, "Undefined property '%s'.", name->chars);
    return false;
  }

  ObjBoundMethod *bound = newBoundMethod(peek(vm, 0), AS_CLOSURE(method));
  pop(vm);
  push(vm, OBJ_VAL(bound));
  return true;
}

bool call(ObjClosure *closure, int argCount, VM *vm) {
  if (argCount != closure->function->arity) {
    runtimeError(vm, "Expected %d arguments but got %d.",
                 closure->function->arity, argCount);
    return false;
  }

  if (vm->frameCount == 64) {
    runtimeError(vm, "Stack overflow.");
    return false;
  }

  CallFrame *frame = &vm->frames[vm->frameCount++];
  frame->closure = closure;
  frame->ip = closure->function->chunk.code;
  frame->slots = vm->stackTop - argCount - 1;
  return true;
}

bool callValue(Value callee, int argCount, VM *vm) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
    case OBJ_BOUND_METHOD: {
      ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
      vm->stackTop[-argCount - 1] = bound->receiver;
      return call(bound->method, argCount, vm);
    }
    case OBJ_CLASS: {
      ObjClass *klass = AS_CLASS(callee);
      vm->stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));
      Value initializer;
      if (tableGet(&klass->methods, copyString("init", 4), &initializer)) {
        return call(AS_CLOSURE(initializer), argCount, vm);
      } else if (argCount != 0) {
        runtimeError(vm, "Expected 0 arguments but got %d.", argCount);
        return false;
      }
      return true;
    }
    case OBJ_CLOSURE:
      return call(AS_CLOSURE(callee), argCount, vm);
    case OBJ_NATIVE: {
      NativeFn native = AS_NATIVE(callee);
      Value result = native(argCount, vm->stackTop - argCount);
      vm->stackTop -= argCount + 1;
      push(vm, result);
      return true;
    }
    default:
      break; // Non-callable object type
    }
  }
  runtimeError(vm, "Can only call functions and classes.");
  return false;
}

bool invokeFromClass(struct ObjClass *klass, ObjString *name,
                            int argCount, VM *vm) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError(vm, "Undefined property '%s'.", name->chars);
    return false;
  }
  return call(AS_CLOSURE(method), argCount, vm);
}

bool invoke(ObjString *name, int argCount, VM *vm) {
  Value receiver = peek(vm, argCount);

  if (!IS_INSTANCE(receiver)) {
    runtimeError(vm, "Only instances have methods.");
    return false;
  }

  struct ObjInstance *instance = AS_INSTANCE(receiver);

  Value value;
  if (tableGet(&instance->fields, name, &value)) {
    vm->stackTop[-argCount - 1] = value;
    return callValue(value, argCount, vm);
  }

  return invokeFromClass(instance->klass, name, argCount, vm);
}
