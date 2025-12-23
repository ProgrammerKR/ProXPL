// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
  Minimal but complete bytecode helper implementation.
  Implements chunk growth, LEB128 emit/read, constants management,
  and basic serializer/deserializer for a single chunk file format.
*/

#include "../../include/bytecode.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

/* --- Chunk and constant table helpers --- */

static void *xrealloc(void *p, size_t s) {
    void *q = realloc(p, s);
    if (!q && s) {
        fprintf(stderr,"out of memory\n");
        abort();
    }
    return q;
}

void chunk_init(Chunk *chunk) {
    initChunk(chunk);
}

void chunk_free(Chunk *chunk) {
    freeChunk(chunk);
}

// Re-using common infrastructure

void chunk_write_byte(Chunk *chunk, uint8_t byte) {
    writeChunk(chunk, byte, 0);
}

void chunk_write_bytes(Chunk *chunk, const uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        writeChunk(chunk, bytes[i], 0);
    }
}

/* --- emission helpers --- */

void emit_opcode(Chunk *chunk, OpCode op) {
    chunk_write_byte(chunk, (uint8_t)op);
}
void emit_u8(Chunk *chunk, uint8_t x) {
    chunk_write_byte(chunk, x);
}
void emit_u16_le(Chunk *chunk, uint16_t x) {
    uint8_t b[2];
    b[0] = x & 0xFF;
    b[1] = (x >> 8) & 0xFF;
    chunk_write_bytes(chunk, b, 2);
}
void emit_u32_le(Chunk *chunk, uint32_t x) {
    uint8_t b[4];
    b[0] = x & 0xFF;
    b[1] = (x >> 8) & 0xFF;
    b[2] = (x >> 16) & 0xFF;
    b[3] = (x >> 24) & 0xFF;
    chunk_write_bytes(chunk, b, 4);
}

/* LEB128 encoders (unsigned and signed) */
void emit_uleb128(Chunk *chunk, uint64_t value) {
    while (1) {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value) {
            byte |= 0x80;
            chunk_write_byte(chunk, byte);
        } else {
            chunk_write_byte(chunk, byte);
            break;
        }
    }
}

void emit_sleb128(Chunk *chunk, int64_t value) {
    int more = 1;
    while (more) {
        uint8_t byte = value & 0x7F;
        int signbit = byte & 0x40;
        value >>= 7;
        if ((value == 0 && !signbit) || (value == -1 && signbit)) {
            more = 0;
        } else {
            byte |= 0x80;
        }
        chunk_write_byte(chunk, byte);
    }
}

/* --- Const table --- */

// Constants managed by ValueArray in Chunk
// Constants managed by ValueArray in Chunk
Value consttable_get(const Chunk *chunk, size_t idx) {
    if (idx >= (size_t)chunk->constants.count) {
        return NIL_VAL;
    }
    return chunk->constants.values[idx];
}

/* --- LEB128 Readers for decoding from buffers --- */

uint64_t read_uleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read) {
    uint64_t result = 0;
    unsigned shift = 0;
    size_t i = 0;
    while (i < buf_len) {
        uint8_t byte = buf[i++];
        result |= (uint64_t)(byte & 0x7F) << shift;
        if (!(byte & 0x80)) {
            if (out_read) *out_read = i;
            return result;
        }
        shift += 7;
    }
    if (out_read) *out_read = i;
    return result;
}

int64_t read_sleb128_from(const uint8_t *buf, size_t buf_len, size_t *out_read) {
    int64_t result = 0;
    unsigned shift = 0;
    size_t i = 0;
    uint8_t byte;
    do {
        if (i >= buf_len) break;
        byte = buf[i++];
        result |= ((int64_t)(byte & 0x7F)) << shift;
        shift += 7;
    } while (byte & 0x80);
    /* sign extend if needed */
    if ((shift < 64) && (byte & 0x40)) {
        result |= -((int64_t)1 << shift);
    }
    if (out_read) *out_read = i;
    return result;
}

/* --- Simple chunk file format serializer/deserializer --- */

/*
File layout (simple single-chunk file):
4 bytes magic "PROX"
2 bytes version (u16 LE)
1 byte endianness (1=LE)
1 byte reserved
4 bytes code_len (u32 LE)
code bytes...
uleb128 constants_count
then each constant:
  u8 type
  payload (type-specific)
Supported constant payloads implemented: Number, String, Bool, Null
FunctionProto not serialized in this minimal serializer (reserved)
*/

static void write_u32_le(FILE *f, uint32_t v) {
    uint8_t b[4] = { v & 0xFF, (v>>8)&0xFF, (v>>16)&0xFF, (v>>24)&0xFF };
    fwrite(b,1,4,f);
}
static uint32_t read_u32_le(FILE *f) {
    uint8_t b[4];
    if (fread(b,1,4,f) != 4) return 0;
    return (uint32_t)b[0] | ((uint32_t)b[1]<<8) | ((uint32_t)b[2]<<16) | ((uint32_t)b[3]<<24);
}

int write_chunk_to_file(const char *path, const Chunk *chunk) {
    FILE *f = fopen(path,"wb");
    if (!f) return -1;
    fwrite("PROX",1,4,f);
    uint16_t ver = 1;
    uint8_t verbuf[2] = { ver & 0xFF, (ver>>8)&0xFF };
    fwrite(verbuf,1,2,f);
    uint8_t endianness = 1;
    fwrite(&endianness,1,1,f);
    uint8_t reserved = 0;
    fwrite(&reserved,1,1,f);
    write_u32_le(f, (uint32_t)chunk->count);
    if (chunk->count) fwrite(chunk->code,1,chunk->count,f);
    /* constants */
    /* write count as uleb128 */
    uint8_t tmp[32];
    uint8_t *p = tmp;
    uint64_t val = chunk->constants.count;
    do {
        uint8_t byte = val & 0x7F;
        val >>= 7;
        if (val) byte |= 0x80;
        *p++ = byte;
    } while (val);
    fwrite(tmp,1,(size_t)(p - tmp), f);
    for (size_t i = 0; i < (size_t)chunk->constants.count; ++i) {
        Value c = chunk->constants.values[i];
        uint8_t type = 0; // mapping for serialization
        if (IS_NIL(c)) type = 1;
        else if (IS_BOOL(c)) type = 2;
        else if (IS_NUMBER(c)) type = 3;
        else if (IS_OBJ(c) && IS_STRING(c)) type = 4;
        
        fwrite(&type,1,1,f);
        if (type == 3) { // VAL_NUMBER equivalent
            /* 8 byte little-endian double */
            union { double d; uint64_t u; } u;
            u.d = AS_NUMBER(c);
            uint8_t b[8];
            for (int k=0; k<8; k++) b[k] = (u.u >> (8*k)) & 0xFF;
            fwrite(b,1,8,f);
        } else if (type == 4) { // VAL_STRING equivalent
            /* write length as uleb128 then bytes */
            size_t len = AS_STRING(c)->length;
            uint8_t tmp2[32];
            uint8_t *q = tmp2;
            uint64_t v2 = len;
            do {
                uint8_t byte = v2 & 0x7F;
                v2 >>= 7;
                if (v2) byte |= 0x80;
                *q++ = byte;
            } while (v2);
            fwrite(tmp2,1,(size_t)(q - tmp2), f);
            if (len) fwrite(AS_CSTRING(c),1,len,f);
        } else if (type == 2) { // VAL_BOOL equivalent
            uint8_t b = AS_BOOL(c) ? 1 : 0;
            fwrite(&b,1,1,f);
        }
    }
    fclose(f);
    return 0;
}

int read_chunk_from_file(const char *path, Chunk *out) {
    FILE *f = fopen(path,"rb");
    if (!f) return -1;
    char magic[4];
    if (fread(magic,1,4,f) != 4) { fclose(f); return -1; }
    if (memcmp(magic,"PROX",4) != 0) { fclose(f); return -1; }
    uint8_t verbuf[2];
    if (fread(verbuf,1,2,f) != 2) { fclose(f); return -1; }
    uint16_t ver = verbuf[0] | (verbuf[1]<<8);
    (void)ver;
    uint8_t endianness;
    if (fread(&endianness,1,1,f) != 1) { fclose(f); return -1; }
    uint8_t reserved;
    if (fread(&reserved,1,1,f) != 1) { fclose(f); return -1; }
    uint32_t code_len = read_u32_le(f);
    chunk_init(out);
    if (code_len) {
        out->code = malloc(code_len);
        if (!out->code) { fclose(f); return -1; }
        if (fread(out->code,1,code_len,f) != code_len) { fclose(f); return -1; }
        out->count = code_len;
        out->capacity = code_len;
    }
    /* read constants_count (uleb128) */
    /* read bytes progressively */
    uint8_t buf[1];
    uint64_t count = 0;
    /* read uleb128 from file stream manually */
    {
        uint64_t result = 0;
        unsigned shift = 0;
        while (1) {
            if (fread(buf,1,1,f) != 1) { fclose(f); return -1; }
            uint8_t byte = buf[0];
            result |= (uint64_t)(byte & 0x7F) << shift;
            if (!(byte & 0x80)) break;
            shift += 7;
        }
        count = result;
    }
    /* read constants */
    initValueArray(&out->constants);
    for (uint64_t i = 0; i < count; ++i) {
        uint8_t type;
        if (fread(&type,1,1,f) != 1) { fclose(f); return -1; }
        Value v = NIL_VAL;
        if (type == 3) { // VAL_NUMBER
            uint8_t b[8];
            if (fread(b,1,8,f) != 8) { fclose(f); return -1; }
            uint64_t u = 0;
            for (int k=0; k<8; k++) u |= ((uint64_t)b[k] << (8*k));
            union { uint64_t u; double d; } uu; uu.u = u;
            v = NUMBER_VAL(uu.d);
        } else if (type == 4) { // VAL_STRING
            /* read uleb128 length */
            uint64_t len = 0;
            unsigned shift = 0;
            while (1) {
                if (fread(buf,1,1,f) != 1) { fclose(f); return -1; }
                uint8_t byte = buf[0];
                len |= (uint64_t)(byte & 0x7F) << shift;
                if (!(byte & 0x80)) break;
                shift += 7;
            }
            char *chars = malloc((size_t)len + 1);
            if (len) {
                if (fread(chars,1,len,f) != len) { free(chars); fclose(f); return -1; }
            }
            chars[len] = '\0';
            v = OBJ_VAL(takeString(chars, (int)len));
        } else if (type == 2) { // VAL_BOOL
            uint8_t b;
            if (fread(&b,1,1,f) != 1) { fclose(f); return -1; }
            v = BOOL_VAL(b ? 1 : 0);
        }
        writeValueArray(&out->constants, v);
    }
    fclose(f);
    return 0;
}

/* --- Test example serializer in this file (exposed for tests) --- */

/* Example: create a chunk for "print('Hello World')" as:
   PUSH_CONST 0  ; "Hello World" at const 0
   CALL (AM_CONST const_idx=1) argc=1   ; assume const 1 is native 'print' function reference
   HALT
*/
int example_write_hello(const char *path) {
    Chunk c;
    chunk_init(&c);
    /* Constants: 0 -> "Hello World", 1 -> symbol 'print' (we'll encode as a string for test) */
    size_t s0 = addConstant(&c, OBJ_VAL(copyString("Hello, World!", 13)));
    size_t s1 = addConstant(&c, OBJ_VAL(copyString("print", 5)));
    (void)s0; (void)s1;

    /* Emit instructions */
#ifndef OP_PUSH_CONST
#define OP_PUSH_CONST OP_CONSTANT
#endif
#ifndef AM_CONST
#define AM_CONST 1
#endif
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 0);
    /* CALL: addressing mode const (AM_CONST), const idx = 1, argc = 1 */
    emit_opcode(&c, OP_CALL);
    emit_u8(&c, AM_CONST);
    emit_opcode(&c, OP_HALT);

    int rc = write_chunk_to_file(path, &c);
    chunk_free(&c);
    return rc;
}

/* Expose example creation for tests */
int example_create_hello_blob(uint8_t **out_buf, size_t *out_len) {
    Chunk c;
    chunk_init(&c);
    addConstant(&c, OBJ_VAL(copyString("Hello, Blob!", 12)));
    addConstant(&c, OBJ_VAL(copyString("print", 5)));
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 0);
    emit_opcode(&c, OP_CALL);
    emit_u8(&c, AM_CONST);
    emit_uleb128(&c, 1);
    emit_u8(&c, 1);
    emit_opcode(&c, OP_HALT);

    /* serialize to memory: mimic write_chunk_to_file but to buffer */
    /* header (12 bytes) + code + encoded constants */
    /* calculate constants uleb128 size */
    uint8_t header[12];
    memcpy(header, "PROX",4);
    header[4] = 1; header[5] = 0; /* version */
    header[6] = 1; header[7] = 0;
    uint32_t code_len = (uint32_t)c.count;
    header[8] = code_len & 0xFF;
    header[9] = (code_len>>8)&0xFF;
    header[10] = (code_len>>16)&0xFF;
    header[11] = (code_len>>24)&0xFF;
    size_t estimated = 12 + c.count + 16 + 64;
    uint8_t *buf = malloc(estimated);
    size_t pos = 0;
    memcpy(buf + pos, header, 12); pos += 12;
    if (c.count) { memcpy(buf + pos, c.code, c.count); pos += c.count; }
    /* constants count uleb */
    uint64_t cc = c.constants.count;
    do {
        uint8_t b = cc & 0x7F;
        cc >>= 7;
        if (cc) b |= 0x80;
        buf[pos++] = b;
    } while (cc);
    for (size_t i = 0; i < (size_t)c.constants.count; ++i) {
        Value v = c.constants.values[i];
        uint8_t type = 0;
        if (IS_NIL(v)) type = 1;
        else if (IS_BOOL(v)) type = 2;
        else if (IS_NUMBER(v)) type = 3;
        else if (IS_OBJ(v) && IS_STRING(v)) type = 4;
        
        buf[pos++] = type;
        if (type == 4) { // String
            uint64_t len = AS_STRING(v)->length;
            do {
                uint8_t b = len & 0x7F;
                len >>= 7;
                if (len) b |= 0x80;
                buf[pos++] = b;
            } while (len);
            memcpy(buf + pos, AS_CSTRING(v), AS_STRING(v)->length); 
            pos += AS_STRING(v)->length;
        } else if (type == 3) { // Number
            union { double d; uint64_t u; } u; 
            u.d = AS_NUMBER(v);
            for (int k=0; k<8; k++) { buf[pos++] = (u.u >> (8*k)) & 0xFF; }
        } else if (type == 2) { // Bool
            buf[pos++] = AS_BOOL(v) ? 1 : 0;
        }
    }
    *out_buf = buf;
    *out_len = pos;
    chunk_free(&c);
    return 0;
}
