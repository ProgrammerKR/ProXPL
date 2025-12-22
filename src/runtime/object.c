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
  }
}

