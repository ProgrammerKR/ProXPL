// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROXPL_BYTECODE_H
#define PROXPL_BYTECODE_H
#include <stdint.h>
#include <stddef.h>

/* Value representation (simple) */
typedef enum {
    VAL_NULL = 0,
    VAL_NUMBER,
    VAL_BOOL,
    VAL_STRING,
    VAL_FUNCTION_PROTO
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        int boolean;
        struct {
            char *chars;
            uint32_t length;
        } string;
        void *proto; /* placeholder for function proto during runtime */
    } as;
} Value;

/* Constant table */
typedef struct {
    Value *items;
    size_t count;
    size_t capacity;
} ConstTable;

/* Chunk (bytecode + constants) */
typedef struct {
    uint8_t *code;
    size_t code_len;
    size_t code_cap;
    ConstTable constants;
} Chunk;

/* Function prototype (serialized form) */
typedef struct {
    uint32_t arity;
    uint16_t max_regs;
    Chunk chunk;
    uint16_t upvalue_count;
} FunctionProto;

/* Opcodes */
typedef enum {
    OP_NOP = 0x00,
    OP_ADD = 0x01,
    OP_SUB = 0x02,
    OP_MUL = 0x03,
    OP_DIV = 0x04,
    OP_MOD = 0x05,
    OP_NEG = 0x06,

    OP_EQ  = 0x10,
    OP_NEQ = 0x11,
    OP_LT  = 0x12,
    OP_LTE = 0x13,
    OP_GT  = 0x14,
    OP_GTE = 0x15,

    OP_NOT = 0x20,
    OP_AND = 0x21,
    OP_OR  = 0x22,

    OP_PUSH_CONST = 0x30,
    OP_LOAD_REG   = 0x31,
    OP_STORE_REG  = 0x32,
    OP_POP        = 0x33,
    OP_DUP        = 0x34,

    OP_JMP         = 0x40,
    OP_JMP_IF_TRUE = 0x41,
    OP_JMP_IF_FALSE= 0x42,

    OP_CALL       = 0x50,
    OP_RETURN     = 0x51,
    OP_TAIL_CALL  = 0x52,

    OP_MAKE_FUNCTION = 0x60,
    OP_CLOSURE       = 0x61,
    OP_CLOSE_UPVALUE = 0x62,
    OP_LOAD_UPVALUE  = 0x63,
    OP_STORE_UPVALUE = 0x64,

    OP_NEW_ARRAY = 0x70,
    OP_INDEX_GET = 0x71,
    OP_INDEX_SET = 0x72,
    OP_GET_FIELD = 0x73,
    OP_SET_FIELD = 0x74,

    OP_DBG_LINE = 0x90,
    OP_DBG_LOC  = 0x91,

    OP_HALT = 0xFF
} Opcode;

/* Addressing modes for CALL / similar */
typedef enum {
    AM_CONST = 0x01,
    AM_REG   = 0x02,
    AM_STACK = 0x03
} AddrMode;

/* Chunk API */
void chunk_init(Chunk *chunk);
void chunk_free(Chunk *chunk);
void chunk_write_byte(Chunk *chunk, uint8_t byte);
void chunk_write_bytes(Chunk *chunk, const uint8_t *bytes, size_t len);

/* emission helpers */
void emit_opcode(Chunk *chunk, Opcode op);
void emit_u8(Chunk *chunk, uint8_t x);
void emit_u16_le(Chunk *chunk, uint16_t x);
void emit_u32_le(Chunk *chunk, uint32_t x);
void emit_uleb128(Chunk *chunk, uint64_t value);
void emit_sleb128(Chunk *chunk, int64_t value);

/* constant table */
void consttable_init(ConstTable *tbl);
void consttable_free(ConstTable *tbl);
size_t consttable_add(ConstTable *tbl, Value v);
Value consttable_get(const ConstTable *tbl, size_t idx);

/* serialization */
int write_chunk_to_file(const char *path, const Chunk *chunk);
int read_chunk_from_file(const char *path, Chunk *out);

/* utility */
uint64_t read_uleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read);
int64_t read_sleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read);

#endif /* PROXPL_BYTECODE_H */
