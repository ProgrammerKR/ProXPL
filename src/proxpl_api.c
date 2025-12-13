/* proxpl_api.c - public API wrappers for stable ABI
 * Exposes a minimal C-style API that callers can use to embed runtime.
 */

#include "proxpl_api.h"
#include "vm.h"
#include "bytecode.h"
#include <stdlib.h>

void proxpl_vm_init(VM *vm) {
    initVM(vm);
}

void proxpl_vm_free(VM *vm) {
    freeVM(vm);
}

InterpretResult proxpl_interpret_chunk(VM *vm, const Chunk *chunk) {
    // interpret expects a mutable chunk; cast away const as the API currently
    return interpret(vm, (Chunk *)chunk);
}

InterpretResult proxpl_interpret_file(VM *vm, const char *path) {
    // Helper: read chunk and interpret
    Chunk c;
    if (read_chunk_from_file(path, &c) != 0) return INTERPRET_COMPILE_ERROR;
    InterpretResult r = interpret(vm, &c);
    chunk_free(&c);
    return r;
}

int proxpl_write_chunk_to_file(const char *path, const Chunk *chunk) {
    return write_chunk_to_file(path, chunk);
}

int proxpl_read_chunk_from_file(const char *path, Chunk *out) {
    return read_chunk_from_file(path, out);
}
