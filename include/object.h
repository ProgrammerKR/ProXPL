// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_OBJECT_H
#define PROX_OBJECT_H

#include "common.h"
#include "value.h"
#include "bytecode.h" 
#include "table.h" 

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

// Macros for Functions
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))

// Macros for Native Functions
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)

// Macros for Modules
#define IS_MODULE(value) isObjType(value, OBJ_MODULE)
#define AS_MODULE(value) ((ObjModule *)AS_OBJ(value))

typedef enum {
  OBJ_STRING,
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_MODULE,
} ObjType;

struct Obj {
  ObjType type;
  bool isMarked;
  struct Obj *next;
};

struct ObjString {
  Obj obj;
  int length;
  uint32_t hash;
  char chars[];
};

struct ObjFunction {
  Obj obj;
  int arity;
  Chunk chunk;
  ObjString *name;
};


// NativeFn moved to common.h

struct ObjNative {
  Obj obj;
  NativeFn function;
};

struct ObjModule {
  Obj obj;
  ObjString *name;
  Table exports; // Symbols exported by this module
};

#ifdef __cplusplus
extern "C" {
#endif

ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
ObjFunction *newFunction();
ObjNative *newNative(NativeFn function);
ObjModule *newModule(ObjString *name);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#ifdef __cplusplus
}
#endif

#endif // PROX_OBJECT_H
