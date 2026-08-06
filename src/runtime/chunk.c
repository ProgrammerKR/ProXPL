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

// Emit unsigned LEB128 for variable-length constant indices
void emit_uleb128(Chunk *chunk, uint64_t value) {
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) byte |= 0x80;
        writeChunk(chunk, byte, 0);
    } while (value != 0);
}

// Emit signed LEB128 (basic implementation)
void emit_sleb128(Chunk *chunk, int64_t value) {
    bool more;
    do {
        uint8_t byte = value & 0x7F;
        int64_t sign_bit = byte & 0x40;
        value >>= 7;
        if ((value == 0 && sign_bit == 0) || (value == -1 && sign_bit != 0)) {
            more = false;
        } else {
            more = true;
            byte |= 0x80;
        }
        writeChunk(chunk, byte, 0);
    } while (more);
}
