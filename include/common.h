// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_COMMON_H
#define PROX_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Type definitions
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif
#define UINT8_COUNT (UINT8_MAX + 1)

typedef uint64_t Value;

// Forward declarations
typedef struct Obj Obj;
typedef struct ObjString ObjString;
typedef struct ObjFunction ObjFunction;
typedef struct ObjNative ObjNative;
typedef struct Table Table;
typedef struct Entry Entry;
typedef struct ValueArray ValueArray;
typedef struct Chunk Chunk;
typedef struct VM VM;
typedef struct CallFrame {
  struct ObjClosure* closure;
  uint8_t* ip;
  Value* slots;
} CallFrame;
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct ExprList ExprList;
typedef struct StmtList StmtList;
typedef struct SwitchCase SwitchCase;
typedef struct SwitchCaseList SwitchCaseList;
typedef struct DictPair DictPair;
typedef struct DictPairList DictPairList;
typedef struct StringList StringList;
// --- Type System ---
typedef struct ObjModule ObjModule;
typedef struct ObjClass ObjClass;
typedef struct ObjInstance ObjInstance;
typedef struct ObjBoundMethod ObjBoundMethod;
typedef struct TypeInfo TypeInfo;

typedef Value (*NativeFn)(int argCount, Value *args);

// --- Cross-Platform API Visibility ---
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  #ifdef proxpl_lib_EXPORTS
    #define PROX_API __declspec(dllexport)
  #else
    #define PROX_API __declspec(dllimport)
  #endif
#else
  #define PROX_API
#endif

// Debugging
#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

#endif // PROX_COMMON_H
