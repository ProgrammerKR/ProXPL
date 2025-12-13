/* test_opcode_roundtrip.c
 * Verifies that a chunk with opcodes and constants round-trips through
 * write_chunk_to_file() and read_chunk_from_file() preserving bytes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bytecode.h"

int main(void) {
    const char *tmp = "tests/tmp_opcode_rt.proxbc";

    Chunk c;
    chunk_init(&c);

    /* Add a couple constants */
    Value v1; v1.type = VAL_NUMBER; v1.as.number = 3.14; consttable_add(&c.constants, v1);
    Value v2; v2.type = VAL_STRING; v2.as.string.length = 5; v2.as.string.chars = strdup("hello"); consttable_add(&c.constants, v2);

    /* Emit instructions */
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 0);
    emit_opcode(&c, OP_PUSH_CONST);
    emit_uleb128(&c, 1);
    emit_opcode(&c, OP_ADD);
    emit_opcode(&c, OP_HALT);

    if (write_chunk_to_file(tmp, &c) != 0) {
        fprintf(stderr, "Failed to write chunk to %s\n", tmp);
        chunk_free(&c);
        return 2;
    }

    Chunk out;
    if (read_chunk_from_file(tmp, &out) != 0) {
        fprintf(stderr, "Failed to read chunk back\n");
        chunk_free(&c);
        return 3;
    }

    /* Compare code bytes */
    if (out.code_len != c.code_len) {
        fprintf(stderr, "Code length mismatch: %zu vs %zu\n", out.code_len, c.code_len);
        chunk_free(&c); chunk_free(&out); return 4;
    }
    if (memcmp(out.code, c.code, c.code_len) != 0) {
        fprintf(stderr, "Code bytes differ\n");
        chunk_free(&c); chunk_free(&out); return 5;
    }

    /* Compare constants count and first values */
    if (out.constants.count != c.constants.count) {
        fprintf(stderr, "Const count mismatch: %zu vs %zu\n", out.constants.count, c.constants.count);
        chunk_free(&c); chunk_free(&out); return 6;
    }
    if (out.constants.items[0].type != VAL_NUMBER) {
        fprintf(stderr, "Const type mismatch for idx 0\n"); chunk_free(&c); chunk_free(&out); return 7;
    }

    /* Clean up */
    chunk_free(&c);
    chunk_free(&out);

    /* Remove temporary file */
    remove(tmp);

    printf("test_opcode_roundtrip: OK\n");
    return 0;
}
