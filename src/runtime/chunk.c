// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdlib.h>

#include "../include/bytecode.h"
#include "memory.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
  chunk->exceptionHandlers.handlers = NULL;
  chunk->exceptionHandlers.count = 0;
  chunk->exceptionHandlers.capacity = 0;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(u8, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  FREE_ARRAY(ExceptionHandler, chunk->exceptionHandlers.handlers, chunk->exceptionHandlers.capacity);
  initChunk(chunk);
}

void writeChunk(Chunk *chunk, u8 byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(u8, chunk->code, oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

int addConstant(Chunk *chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void addExceptionHandler(Chunk *chunk, size_t start, size_t end, size_t handler) {
  if (chunk->exceptionHandlers.capacity < chunk->exceptionHandlers.count + 1) {
    int oldCapacity = chunk->exceptionHandlers.capacity;
    chunk->exceptionHandlers.capacity = GROW_CAPACITY(oldCapacity);
    chunk->exceptionHandlers.handlers = GROW_ARRAY(ExceptionHandler, chunk->exceptionHandlers.handlers, oldCapacity, chunk->exceptionHandlers.capacity);
  }
  chunk->exceptionHandlers.handlers[chunk->exceptionHandlers.count].start_ip = start;
  chunk->exceptionHandlers.handlers[chunk->exceptionHandlers.count].end_ip = end;
  chunk->exceptionHandlers.handlers[chunk->exceptionHandlers.count].handler_ip = handler;
  chunk->exceptionHandlers.count++;
}
