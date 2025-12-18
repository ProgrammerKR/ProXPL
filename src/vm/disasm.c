// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
  Disassembler utility: reads a chunk and prints a human-readable
  listing of instructions with constant annotations.
*/
#include "../../include/bytecode.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

/* helper to read a uleb128 from memory buffer at pos */
static uint64_t read_uleb128_buf(const uint8_t *buf, size_t buf_len, size_t *pos) {
    uint64_t result = 0;
    unsigned shift = 0;
    size_t i = *pos;
    while (i < buf_len) {
        uint8_t byte = buf[i++];
        result |= (uint64_t)(byte & 0x7F) << shift;
        if (!(byte & 0x80)) {
            *pos = i;
            return result;
        }
        shift += 7;
    }
    *pos = i;
    return result;
}

static int64_t read_sleb128_buf(const uint8_t *buf, size_t buf_len, size_t *pos) {
    int64_t result = 0;
    unsigned shift = 0;
    size_t i = *pos;
    uint8_t byte = 0;
    do {
        if (i >= buf_len) { *pos = i; return 0; }
        byte = buf[i++];
        result |= ((int64_t)(byte & 0x7F)) << shift;
        shift += 7;
    } while (byte & 0x80);
    if ((shift < 64) && (byte & 0x40)) {
        result |= -((int64_t)1 << shift);
    }
    *pos = i;
    return result;
}

static const char *val_to_str(const Value *v, char *buf, size_t bufsize) {
    if (!v) { snprintf(buf, bufsize, "null"); return buf; }
    switch (v->type) {
        case VAL_NULL: snprintf(buf, bufsize, "null"); break;
        case VAL_NUMBER: snprintf(buf, bufsize, "%g", v->as.number); break;
        case VAL_BOOL: snprintf(buf, bufsize, v->as.boolean ? "true":"false"); break;
        case VAL_STRING: {
            size_t n = v->as.string.length;
            if (n > bufsize-3) n = bufsize-3;
            snprintf(buf, bufsize, "\"%.*s\"", (int)n, v->as.string.chars);
        } break;
        default: snprintf(buf, bufsize, "<const type %d>", (int)v->type); break;
    }
    return buf;
}

void disasm_chunk(const Chunk *chunk) {
    const uint8_t *code = chunk->code;
    size_t len = chunk->code_len;
    size_t pc = 0;
    while (pc < len) {
        uint8_t op = code[pc++];
        printf("%04zu: ", pc-1);
        switch (op) {
            case OP_PUSH_CONST: {
                size_t start = pc;
                uint64_t idx = read_uleb128_buf(code, len, &pc);
                char tmp[128]; const Value v = consttable_get(&chunk->constants, (size_t)idx);
                printf("PUSH_CONST %llu ; %s\n", (unsigned long long)idx, val_to_str(&v,tmp,sizeof(tmp)));
            } break;
            case OP_CALL: {
                if (pc >= len) { printf("CALL <truncated>\n"); break; }
                uint8_t am = code[pc++];
                if (am == AM_CONST) {
                    uint64_t idx = read_uleb128_buf(code, len, &pc);
                    if (pc >= len) { printf("CALL AM_CONST %llu <trunc>\n", (unsigned long long)idx); break; }
                    uint8_t argc = code[pc++];
                    const Value v = consttable_get(&chunk->constants, (size_t)idx);
                    char tmp[128]; printf("CALL AM_CONST %llu argc=%u ; target=%s\n", (unsigned long long)idx, (unsigned)argc, val_to_str(&v,tmp,sizeof(tmp)));
                } else if (am == AM_REG) {
                    uint8_t reg = code[pc++];
                    uint8_t argc = code[pc++];
                    printf("CALL AM_REG r%u argc=%u\n", (unsigned)reg, (unsigned)argc);
                } else {
                    uint64_t idx = read_uleb128_buf(code, len, &pc);
                    uint8_t argc = code[pc++];
                    printf("CALL AM_STACK idx=%llu argc=%u\n", (unsigned long long)idx, (unsigned)argc);
                }
            } break;
            case OP_HALT:
                printf("HALT\n"); break;
            case OP_ADD: {
                if (pc + 3 <= len) {
                    uint8_t ra = code[pc++], rb = code[pc++], rc = code[pc++];
                    printf("ADD r%u, r%u, r%u\n", ra, rb, rc);
                } else printf("ADD <trunc>\n");
            } break;
            case OP_JMP: {
                size_t saved = pc;
                int64_t off = read_sleb128_buf(code, len, &pc);
                printf("JMP %lld (to %zu)\n", (long long)off, (pc + off));
            } break;
            case OP_POP: printf("POP\n"); break;
            case OP_DUP: printf("DUP\n"); break;
            case OP_PUSH_CONST + 1: printf("unknown\n"); break;
            default:
                printf("OP_0x%02X\n", op);
        }
    }
}
