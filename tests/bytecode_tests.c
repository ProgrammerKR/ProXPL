/*
  Test runner for bytecode serialization, deserialization, disassembly,
  and a minimal dispatch execution (no GC).

  Builds and runs several example bytecode blobs, disassembles them,
  and executes them with the simple test VM (supports CALL print).
*/

#include "../include/bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* forward declares from implementation files */
int example_write_hello(const char *path);
int example_create_hello_blob(uint8_t **out_buf, size_t *out_len);
int read_chunk_from_file(const char *path, Chunk *out);
int write_chunk_to_file(const char *path, const Chunk *chunk);
void disasm_chunk(const Chunk *chunk);
int vm_run_chunk_simple(const Chunk *chunk);

int main(void) {
    printf("ProXPL bytecode tests start\n");

    /* Create an example hello chunk and write to file */
    const char *outpath = "examples/hello.proxbc";
    if (example_write_hello(outpath) == 0) {
        printf("Wrote example chunk to %s\n", outpath);
    } else {
        printf("Failed to write example chunk\n");
    }

    /* Read it back */
    Chunk c;
    if (read_chunk_from_file(outpath, &c) != 0) {
        fprintf(stderr,"Failed to read chunk back\n");
        return 1;
    }
    printf("Disassembly of read chunk:\n");
    disasm_chunk(&c);

    /* Execute with simple VM */
    printf("Executing chunk (simple VM):\n");
    int rc = vm_run_chunk_simple(&c);
    if (rc != 0) {
        fprintf(stderr,"Execution failed with rc=%d\n", rc);
    } else {
        printf("Execution finished successfully\n");
    }

    /* Cleanup */
    chunk_free(&c);

    /* Create blob in-memory and disasm */
    uint8_t *buf = NULL; size_t buflen = 0;
    example_create_hello_blob(&buf, &buflen);
    if (buf && buflen) {
        /* write to temporary file to reuse loader */
        const char *tmp = "examples/hello_blob.proxbc";
        FILE *f = fopen(tmp,"wb");
        if (f) { fwrite(buf,1,buflen,f); fclose(f); printf("Wrote blob to %s\n", tmp); }
        free(buf);
        Chunk c2;
        if (read_chunk_from_file(tmp, &c2) == 0) {
            printf("Disassembling blob chunk:\n");
            disasm_chunk(&c2);
            printf("Running blob chunk:\n");
            vm_run_chunk_simple(&c2);
            chunk_free(&c2);
        }
    }
    printf("ProXPL bytecode tests finished\n");
    return 0;
}
