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
  initChunk(&function->chunk); // Requires chunk.h
  return function;
}

ObjNative *newNative(NativeFn function) {
  ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}

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
  case OBJ_INSTANCE:
    printf("<instance %s>", ((struct ObjInstance*)AS_OBJ(value))->klass->name->chars);
    break;
  case OBJ_BOUND_METHOD:
    printObject(OBJ_VAL(((struct ObjBoundMethod*)AS_OBJ(value))->method->function));
    break;
  case OBJ_LIST:
    printf("[list]");
    break;
  case OBJ_DICTIONARY:
    printf("{dictionary}");
    break;
  }
}

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
  return klass;
}

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

