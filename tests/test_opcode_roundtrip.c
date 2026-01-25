// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/* test_opcode_roundtrip.c
 * Verifies that a chunk with opcodes and constants round-trips through
 * write_chunk_to_file() and read_chunk_from_file() preserving bytes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bytecode.h"

#include "../include/value.h"
#include "../include/object.h"

// Stub for native functions? No, linked with object.c which needs vm
// So we need vm stub or link with vm_stub.o

int main(void) {
    const char *tmp = "tests/tmp_opcode_rt.proxbc";

    Chunk c;
    initChunk(&c);

    /* Add a couple constants */
    /* Note: copyString requires VM to be zero-initialized (vm global) which is done by BSS or explicit init if linked */
    Value v1 = NUMBER_VAL(3.14);
    addConstant(&c, v1);
    
    Value v2 = OBJ_VAL(copyString("hello", 5));
    addConstant(&c, v2);

    /* Emit instructions */
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 0);
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 1);
    emit_opcode(&c, OP_ADD);
    emit_opcode(&c, OP_HALT);

    if (write_chunk_to_file(tmp, &c) != 0) {
        fprintf(stderr, "Failed to write chunk to %s\n", tmp);
        freeChunk(&c);
        return 2;
    }

    Chunk out;
    if (read_chunk_from_file(tmp, &out) != 0) {
        fprintf(stderr, "Failed to read chunk back\n");
        freeChunk(&c);
        return 3;
    }

    /* Compare code bytes */
    if (out.count != c.count) {
        fprintf(stderr, "Code length mismatch: %d vs %d\n", out.count, c.count);
        freeChunk(&c); freeChunk(&out); return 4;
    }
    if (memcmp(out.code, c.code, c.count) != 0) {
        fprintf(stderr, "Code bytes differ\n");
        freeChunk(&c); freeChunk(&out); return 5;
    }

    /* Compare constants count and first values */
    if (out.constants.count != c.constants.count) {
        fprintf(stderr, "Const count mismatch: %d vs %d\n", out.constants.count, c.constants.count);
        freeChunk(&c); freeChunk(&out); return 6;
    }
    // Check first constant is number
    Value out_v1 = out.constants.values[0];
    if (!IS_NUMBER(out_v1)) {
        fprintf(stderr, "Const type mismatch for idx 0\n"); freeChunk(&c); freeChunk(&out); return 7;
    }
    if (AS_NUMBER(out_v1) != 3.14) {
         fprintf(stderr, "Const value mismatch for idx 0\n"); freeChunk(&c); freeChunk(&out); return 8;
    }

    /* Clean up */
    freeChunk(&c);
    freeChunk(&out);

    /* Remove temporary file */
    remove(tmp);

    printf("test_opcode_roundtrip: OK\n");
    return 0;
}
