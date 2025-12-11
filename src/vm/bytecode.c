/*
  Minimal but complete bytecode helper implementation.
  Implements chunk growth, LEB128 emit/read, constants management,
  and basic serializer/deserializer for a single chunk file format.
*/

#include "../../include/bytecode.h"
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
    chunk->code = NULL;
    chunk->code_len = 0;
    chunk->code_cap = 0;
    consttable_init(&chunk->constants);
}

void chunk_free(Chunk *chunk) {
    if (chunk->code) free(chunk->code);
    chunk->code = NULL;
    chunk->code_len = 0;
    chunk->code_cap = 0;
    consttable_free(&chunk->constants);
}

/* ensure capacity */
static void chunk_ensure(Chunk *chunk, size_t want) {
    if (chunk->code_cap >= want) return;
    size_t cap = chunk->code_cap ? chunk->code_cap : 64;
    while (cap < want) cap *= 2;
    chunk->code = xrealloc(chunk->code, cap);
    chunk->code_cap = cap;
}

void chunk_write_byte(Chunk *chunk, uint8_t byte) {
    chunk_ensure(chunk, chunk->code_len + 1);
    chunk->code[chunk->code_len++] = byte;
}

void chunk_write_bytes(Chunk *chunk, const uint8_t *bytes, size_t len) {
    chunk_ensure(chunk, chunk->code_len + len);
    memcpy(chunk->code + chunk->code_len, bytes, len);
    chunk->code_len += len;
}

/* --- emission helpers --- */

void emit_opcode(Chunk *chunk, Opcode op) {
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

void consttable_init(ConstTable *tbl) {
    tbl->items = NULL;
    tbl->count = 0;
    tbl->capacity = 0;
}
void consttable_free(ConstTable *tbl) {
    if (!tbl) return;
    for (size_t i = 0; i < tbl->count; ++i) {
        if (tbl->items[i].type == VAL_STRING) {
            free(tbl->items[i].as.string.chars);
        } else if (tbl->items[i].type == VAL_FUNCTION_PROTO) {
            /* if we had nested protos, we'd free them */
        }
    }
    free(tbl->items);
    tbl->items = NULL;
    tbl->count = 0;
    tbl->capacity = 0;
}
size_t consttable_add(ConstTable *tbl, Value v) {
    if (tbl->count + 1 > tbl->capacity) {
        size_t cap = tbl->capacity ? tbl->capacity * 2 : 8;
        tbl->items = xrealloc(tbl->items, cap * sizeof(Value));
        tbl->capacity = cap;
    }
    tbl->items[tbl->count] = v;
    return tbl->count++;
}
Value consttable_get(const ConstTable *tbl, size_t idx) {
    if (idx >= tbl->count) {
        Value nullv;
        nullv.type = VAL_NULL;
        return nullv;
    }
    return tbl->items[idx];
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
    write_u32_le(f, (uint32_t)chunk->code_len);
    if (chunk->code_len) fwrite(chunk->code,1,chunk->code_len,f);
    /* constants */
    /* write count as uleb128 */
    uint8_t tmp[32];
    /* create a temp buffer to use emit_uleb128 into and write; reuse chunk emission helpers temporarily */
    /* We'll encode uleb into tmp */
    uint8_t *p = tmp;
    uint64_t val = chunk->constants.count;
    do {
        uint8_t byte = val & 0x7F;
        val >>= 7;
        if (val) byte |= 0x80;
        *p++ = byte;
    } while (val);
    fwrite(tmp,1,(size_t)(p - tmp), f);
    for (size_t i = 0; i < chunk->constants.count; ++i) {
        Value c = chunk->constants.items[i];
        fwrite(&c.type,1,1,f);
        switch (c.type) {
            case VAL_NUMBER: {
                /* 8 byte little-endian double */
                union { double d; uint64_t u; } u;
                u.d = c.as.number;
                uint8_t b[8];
                b[0] = u.u & 0xFF;
                b[1] = (u.u>>8)&0xFF;
                b[2] = (u.u>>16)&0xFF;
                b[3] = (u.u>>24)&0xFF;
                b[4] = (u.u>>32)&0xFF;
                b[5] = (u.u>>40)&0xFF;
                b[6] = (u.u>>48)&0xFF;
                b[7] = (u.u>>56)&0xFF;
                fwrite(b,1,8,f);
            } break;
            case VAL_STRING: {
                /* write length as uleb128 then bytes */
                size_t len = c.as.string.length;
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
                if (len) fwrite(c.as.string.chars,1,len,f);
            } break;
            case VAL_BOOL: {
                uint8_t b = c.as.boolean ? 1 : 0;
                fwrite(&b,1,1,f);
            } break;
            case VAL_NULL: {
                /* nothing */
            } break;
            default:
                /* skip unsupported types */
                break;
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
    fread(&endianness,1,1,f);
    uint8_t reserved;
    fread(&reserved,1,1,f);
    uint32_t code_len = read_u32_le(f);
    chunk_init(out);
    if (code_len) {
        out->code = malloc(code_len);
        if (!out->code) { fclose(f); return -1; }
        if (fread(out->code,1,code_len,f) != code_len) { fclose(f); return -1; }
        out->code_len = code_len;
        out->code_cap = code_len;
    }
    /* read constants_count (uleb128) */
    /* read bytes progressively */
    uint8_t buf[1];
    size_t readbytes = 0;
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
    consttable_init(&out->constants);
    for (uint64_t i = 0; i < count; ++i) {
        uint8_t type;
        if (fread(&type,1,1,f) != 1) { fclose(f); return -1; }
        Value v;
        v.type = (ValueType)type;
        if (v.type == VAL_NUMBER) {
            uint8_t b[8];
            if (fread(b,1,8,f) != 8) { fclose(f); return -1; }
            uint64_t u = ((uint64_t)b[0]) | ((uint64_t)b[1]<<8) | ((uint64_t)b[2]<<16) | ((uint64_t)b[3]<<24)
                       | ((uint64_t)b[4]<<32) | ((uint64_t)b[5]<<40) | ((uint64_t)b[6]<<48) | ((uint64_t)b[7]<<56);
            union { uint64_t u; double d; } uu; uu.u = u;
            v.as.number = uu.d;
        } else if (v.type == VAL_STRING) {
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
            v.as.string.length = (uint32_t)len;
            v.as.string.chars = malloc((size_t)len + 1);
            if (len) {
                if (fread(v.as.string.chars,1,len,f) != len) { fclose(f); return -1; }
            }
            v.as.string.chars[len] = '\0';
        } else if (v.type == VAL_BOOL) {
            uint8_t b;
            if (fread(&b,1,1,f) != 1) { fclose(f); return -1; }
            v.as.boolean = b ? 1 : 0;
        } else if (v.type == VAL_NULL) {
            /* nothing */
        } else {
            /* unknown type: treat as null and skip */
            v.type = VAL_NULL;
        }
        consttable_add(&out->constants, v);
    }
    fclose(f);
    return 0;
}

/* --- Test example serializer in this file (exposed for tests) --- */

/* Helper for creating a string constant */
Value make_string_const(const char *s) {
    Value v;
    v.type = VAL_STRING;
    size_t len = s ? strlen(s) : 0;
    v.as.string.length = (uint32_t)len;
    v.as.string.chars = malloc(len + 1);
    if (len) memcpy(v.as.string.chars, s, len);
    v.as.string.chars[len] = '\0';
    return v;
}

Value make_number_const(double d) {
    Value v;
    v.type = VAL_NUMBER;
    v.as.number = d;
    return v;
}

Value make_bool_const(int b) {
    Value v;
    v.type = VAL_BOOL;
    v.as.boolean = b ? 1 : 0;
    return v;
}

Value make_null_const(void) {
    Value v;
    v.type = VAL_NULL;
    return v;
}

/* Example: create a chunk for "print('Hello World')" as:
   PUSH_CONST 0  ; "Hello World" at const 0
   CALL (AM_CONST const_idx=1) argc=1   ; assume const 1 is native 'print' function reference
   HALT
*/
int example_write_hello(const char *path) {
    Chunk c;
    chunk_init(&c);
    /* Constants: 0 -> "Hello World", 1 -> symbol 'print' (we'll encode as a string for test) */
    size_t s0 = consttable_add(&c.constants, make_string_const("Hello, World!"));
    size_t s1 = consttable_add(&c.constants, make_string_const("print"));
    (void)s0; (void)s1;

    /* Emit instructions */
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 0);
    /* CALL: addressing mode const (AM_CONST), const idx = 1, argc = 1 */
    emit_opcode(&c, OP_CALL);
    emit_u8(&c, AM_CONST);
    emit_uleb128(&c, 1);
    emit_u8(&c, 1);
    emit_opcode(&c, OP_HALT);

    int rc = write_chunk_to_file(path, &c);
    chunk_free(&c);
    return rc;
}

/* Expose example creation for tests */
int example_create_hello_blob(uint8_t **out_buf, size_t *out_len) {
    Chunk c;
    chunk_init(&c);
    consttable_add(&c.constants, make_string_const("Hello, Blob!"));
    consttable_add(&c.constants, make_string_const("print"));
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
    uint32_t code_len = (uint32_t)c.code_len;
    header[8] = code_len & 0xFF;
    header[9] = (code_len>>8)&0xFF;
    header[10] = (code_len>>16)&0xFF;
    header[11] = (code_len>>24)&0xFF;
    size_t estimated = 12 + c.code_len + 16 + 64;
    uint8_t *buf = malloc(estimated);
    size_t pos = 0;
    memcpy(buf + pos, header, 12); pos += 12;
    if (c.code_len) { memcpy(buf + pos, c.code, c.code_len); pos += c.code_len; }
    /* constants count uleb */
    uint64_t cc = c.constants.count;
    do {
        uint8_t b = cc & 0x7F;
        cc >>= 7;
        if (cc) b |= 0x80;
        buf[pos++] = b;
    } while (cc);
    for (size_t i = 0; i < c.constants.count; ++i) {
        Value v = c.constants.items[i];
        buf[pos++] = (uint8_t)v.type;
        if (v.type == VAL_STRING) {
            /* length uleb */
            uint64_t len = v.as.string.length;
            do {
                uint8_t b = len & 0x7F;
                len >>= 7;
                if (len) b |= 0x80;
                buf[pos++] = b;
            } while (len);
            memcpy(buf + pos, v.as.string.chars, v.as.string.length); pos += v.as.string.length;
        } else if (v.type == VAL_NUMBER) {
            union { double d; uint64_t u; } u; u.d = v.as.number;
            for (int k=0;k<8;k++) { buf[pos++] = (u.u >> (8*k)) & 0xFF; }
        } else if (v.type == VAL_BOOL) {
            buf[pos++] = v.as.boolean ? 1 : 0;
        } else if (v.type == VAL_NULL) {
            /* nothing */
        }
    }
    *out_buf = buf;
    *out_len = pos;
    chunk_free(&c);
    return 0;
}
