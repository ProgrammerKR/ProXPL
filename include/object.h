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

#define IS_UPVALUE(value) isObjType(value, OBJ_UPVALUE)
#define AS_UPVALUE(value) ((ObjUpvalue *)AS_OBJ(value))

#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))

#define IS_CLASS(value) isObjType(value, OBJ_CLASS)
#define AS_CLASS(value) ((struct ObjClass *)AS_OBJ(value))

#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)
#define AS_INSTANCE(value) ((struct ObjInstance *)AS_OBJ(value))

#define IS_BOUND_METHOD(value) isObjType(value, OBJ_BOUND_METHOD)
#define AS_BOUND_METHOD(value) ((struct ObjBoundMethod *)AS_OBJ(value))

#define IS_TASK(value) isObjType(value, OBJ_TASK)
#define AS_TASK(value) ((struct ObjTask *)AS_OBJ(value))

#define IS_LIST(value) isObjType(value, OBJ_LIST)
#define AS_LIST(value) ((struct ObjList *)AS_OBJ(value))

#define IS_DICTIONARY(value) isObjType(value, OBJ_DICTIONARY)
#define AS_DICTIONARY(value) ((struct ObjDictionary *)AS_OBJ(value))

#define IS_INTERFACE(value) isObjType(value, OBJ_INTERFACE)
#define AS_INTERFACE(value) ((ObjInterface *)AS_OBJ(value))

typedef enum {
  OBJ_STRING,
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_MODULE,
  OBJ_UPVALUE,
  OBJ_CLOSURE,
  OBJ_CLASS,
  OBJ_INSTANCE,
  OBJ_BOUND_METHOD,
  OBJ_LIST,
  OBJ_DICTIONARY,

  OBJ_TASK,
  OBJ_INTERFACE
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
  int upvalueCount;
  Chunk chunk;
  ObjString *name;
  AccessLevel access;
  bool isStatic;
  bool isAbstract;
  struct ObjClass *ownerClass;
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

typedef struct ObjUpvalue {
  Obj obj;
  Value *location;
  Value closed;
  struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct ObjClosure {
  Obj obj;
  ObjFunction *function;
  ObjUpvalue **upvalues;
  int upvalueCount;
} ObjClosure;

// Interfaces are essentially named method tables?
// Or just type markers? For now, simple marker with name.
typedef struct ObjInterface {
  Obj obj;
  ObjString *name;
  Table methods; 
} ObjInterface;

struct ObjClass {
  Obj obj;
  ObjString *name;
  Table methods;
  int interfaceCount;
  Value *interfaces; 
};

struct ObjInstance {
  Obj obj;
  struct ObjClass *klass; // 'class' is a keyword in C++
  Table fields;
};

struct ObjBoundMethod {
  Obj obj;
  Value receiver;
  ObjClosure *method;
};

struct ObjList {
  Obj obj;
  int count;
  int capacity;
  Value *items;
};

struct ObjDictionary {
  Obj obj;
  Table items;
};

typedef struct ObjTask ObjTask;

typedef void (*ResumeFn)(void*);

struct ObjTask {
  Obj obj;
  void* coroHandle; // LLVM Coroutine Handle
  ResumeFn resume;  // Helper to resume coroutine
  bool completed;
  Value result;
  struct ObjTask* next; // For scheduler queue
};

#ifdef __cplusplus
extern "C" {
#endif

ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
ObjFunction *newFunction();
ObjNative *newNative(NativeFn function);

ObjModule *newModule(ObjString *name);
ObjClosure *newClosure(ObjFunction *function);
ObjUpvalue *newUpvalue(Value *slot);
struct ObjClass *newClass(ObjString *name);
struct ObjInterface *newInterface(ObjString *name);
struct ObjInstance *newInstance(struct ObjClass *klass);
struct ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method);
struct ObjList *newList();
struct ObjDictionary *newDictionary();
struct ObjTask *newTask(void* hdl, ResumeFn resume);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#ifdef __cplusplus
}
#endif

#endif // PROX_OBJECT_H
