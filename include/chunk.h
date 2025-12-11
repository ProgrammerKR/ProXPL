#ifndef PROX_CHUNK_H
#define PROX_CHUNK_H

#include "common.h"
#include "value.h"

// OpCodes
typedef enum {
  OP_CONSTANT,
  OP_TRUE,
  OP_FALSE,
  OP_NULL,
  OP_POP,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN,
} OpCode;

// Chunk: Sequence of bytecode
typedef struct {
  int count;
  int capacity;
  u8 *code;
  int *lines; // Line number for each byte
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, u8 byte, int line);
int addConstant(Chunk *chunk, Value value);

#endif
