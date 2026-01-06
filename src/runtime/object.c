// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"
#include "../include/object.h"
#include "../include/value.h"
#include "../include/vm.h"
#include "../include/bytecode.h" 

// This is the critical fix for the "vm undeclared" error:
extern VM vm; 

#define ALLOCATE_OBJ(type, objectType) \
  (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type) {
  Obj *object = (Obj *)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects; // Now works because 'vm' is declared extern above
  vm.objects = object;
  return object;
}

ObjFunction *newFunction() {
  ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->arity = 0;
  function->upvalueCount = 0;
  function->name = NULL;
<<<<<<< HEAD
  function->access = ACCESS_PUBLIC;
  function->isStatic = false;
  function->isAbstract = false;
=======
>>>>>>> fix-ci-build
  initChunk(&function->chunk); // Requires chunk.h
  return function;
}

ObjNative *newNative(NativeFn function) {
  ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}

<<<<<<< HEAD
ObjForeign *newForeign(ObjString* name, void* library, void* function) {
  ObjForeign *foreign = ALLOCATE_OBJ(ObjForeign, OBJ_FOREIGN);
  foreign->name = name;
  foreign->library = library;
  foreign->function = function;
  return foreign;
}

=======
>>>>>>> fix-ci-build
ObjModule *newModule(ObjString *name) {
  ObjModule *module = ALLOCATE_OBJ(ObjModule, OBJ_MODULE);
  module->name = name;
  initTable(&module->exports);
  return module;
}

static ObjString *allocateString(char *chars, int length, uint32_t hash) {
  ObjString *string = (ObjString *)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
  string->length = length;
  string->hash = hash;
  memcpy(string->chars, chars, length);
  string->chars[length] = '\0';
  
  // Intern the string
  tableSet(&vm.strings, string, NIL_VAL);
  
  return string;
}

static uint32_t hashString(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString *takeString(char *chars, int length) {
  uint32_t hash = hashString(chars, length);
  
  ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL) {
      FREE_ARRAY(char, chars, length + 1);
      return interned;
  }

  // With Flexible Array Member, we cannot "take" the pointer. We must copy.
  // allocateString copies 'chars', so we must free the original 'chars' now.
  ObjString* string = allocateString(chars, length, hash);
  FREE_ARRAY(char, chars, length + 1);
  return string;
}

ObjString *copyString(const char *chars, int length) {
  uint32_t hash = hashString(chars, length);
  
  // Check if string is already interned
  ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL) return interned;

  return allocateString((char*)chars, length, hash);
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
  case OBJ_STRING:
    printf("%s", AS_CSTRING(value));
    break;
  case OBJ_FUNCTION: {
    ObjFunction *function = (ObjFunction *)AS_OBJ(value);
    if (function->name == NULL) {
      printf("<script>");
      return;
    }
    printf("<fn %s>", function->name->chars);
    break;
  }
  case OBJ_NATIVE:
    printf("<native fn>");
    break;
  case OBJ_MODULE:
    printf("<module %s>", ((ObjModule*)AS_OBJ(value))->name->chars);
    break;
  case OBJ_CLOSURE:
    printObject(OBJ_VAL(((ObjClosure*)AS_OBJ(value))->function));
    break;
  case OBJ_UPVALUE:
    printf("upvalue");
    break;
  case OBJ_CLASS:
    printf("<class %s>", ((struct ObjClass*)AS_OBJ(value))->name->chars);
    break;
<<<<<<< HEAD
  case OBJ_INTERFACE:
    printf("<interface %s>", ((ObjInterface*)AS_OBJ(value))->name->chars);
    break;
=======
>>>>>>> fix-ci-build
  case OBJ_INSTANCE:
    printf("<instance %s>", ((struct ObjInstance*)AS_OBJ(value))->klass->name->chars);
    break;
  case OBJ_BOUND_METHOD:
    printObject(OBJ_VAL(((struct ObjBoundMethod*)AS_OBJ(value))->method->function));
    break;
<<<<<<< HEAD
  case OBJ_FOREIGN:
    printf("<foreign fn %s>", ((ObjForeign*)AS_OBJ(value))->name->chars);
    break;
  case OBJ_LIST:
    printf("[list]");
    break;
  case OBJ_DICTIONARY:
    printf("{dictionary}");
    break;
  case OBJ_TASK:
    printf("<task>");
    break;
  }
}

struct ObjTask *newTask(void* hdl, ResumeFn resume) {
  ObjTask *task = (ObjTask *)allocateObject(sizeof(ObjTask), OBJ_TASK);
  task->coroHandle = hdl;
  task->resume = resume;
  task->completed = false;
  task->result = NULL_VAL;
  task->next = NULL;
  return task;
}

=======
  }
}

>>>>>>> fix-ci-build
ObjClosure *newClosure(ObjFunction *function) {
  ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
  for (int i = 0; i < function->upvalueCount; i++) {
    upvalues[i] = NULL;
  }

  ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;
  closure->upvalues = upvalues;
  closure->upvalueCount = function->upvalueCount;
  return closure;
}

ObjUpvalue *newUpvalue(Value *slot) {
  ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
  upvalue->closed = NIL_VAL;
  upvalue->location = slot;
  upvalue->next = NULL;
  return upvalue;
}

struct ObjClass *newClass(ObjString *name) {
  struct ObjClass *klass = ALLOCATE_OBJ(struct ObjClass, OBJ_CLASS);
  klass->name = name;
  initTable(&klass->methods);
<<<<<<< HEAD
  klass->interfaceCount = 0;
  klass->interfaces = NULL;
  return klass;
}

struct ObjInterface *newInterface(ObjString *name) {
  struct ObjInterface *interface = ALLOCATE_OBJ(struct ObjInterface, OBJ_INTERFACE);
  interface->name = name;
  initTable(&interface->methods);
  return interface;
}

=======
  return klass;
}

>>>>>>> fix-ci-build
struct ObjInstance *newInstance(struct ObjClass *klass) {
  struct ObjInstance *instance = ALLOCATE_OBJ(struct ObjInstance, OBJ_INSTANCE);
  instance->klass = klass;
  initTable(&instance->fields);
  return instance;
}

struct ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method) {
  struct ObjBoundMethod *bound = ALLOCATE_OBJ(struct ObjBoundMethod, OBJ_BOUND_METHOD);
  bound->receiver = receiver;
  bound->method = method;
  return bound;
}

<<<<<<< HEAD
struct ObjList *newList() {
  struct ObjList *list = ALLOCATE_OBJ(struct ObjList, OBJ_LIST);
  list->count = 0;
  list->capacity = 0;
  list->items = NULL;
  return list;
}

struct ObjDictionary *newDictionary() {
  struct ObjDictionary *dict = ALLOCATE_OBJ(struct ObjDictionary, OBJ_DICTIONARY);
  initTable(&dict->items);
  return dict;
}

=======
>>>>>>> fix-ci-build
