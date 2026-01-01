// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_BYTECODE_H
#define PROX_BYTECODE_H

#include "common.h"
#include "value.h"

#define AM_CONST 1

// OpCodes
typedef enum {
  OP_CONSTANT,
  OP_NOP,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_DUP,
  OP_BUILD_LIST,
  OP_BUILD_MAP,
  OP_GET_INDEX,
  OP_SET_INDEX,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  OP_GET_SUPER,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_LOOP,
  OP_CALL,
  OP_INVOKE,
  OP_SUPER_INVOKE,
  OP_CLOSURE,
  OP_CLOSE_UPVALUE,
  OP_RETURN,
  OP_CLASS,
  OP_INHERIT,
  OP_METHOD,
  OP_USE,
  OP_TRY,
  OP_CATCH,

  OP_END_TRY,
  OP_INTERFACE,
  OP_IMPLEMENT,
  OP_HALT = 0xFF
} OpCode;

struct Chunk {
  int count;
  int capacity;
  uint8_t *code;
  int *lines;
  ValueArray constants;
};

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);
Value consttable_get(const Chunk *chunk, size_t idx);

// File serialization
int write_chunk_to_file(const char *path, const Chunk *chunk);
int read_chunk_from_file(const char *path, Chunk *out);

// LEB128 Utilities
uint64_t read_uleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read);
int64_t read_sleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read);

#endif // PROX_BYTECODE_H
