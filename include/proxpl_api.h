#ifndef PROXPL_API_H
#define PROXPL_API_H

#include "vm.h"
#include "chunk.h"

/* Public API: Initialize and shutdown the VM */
void proxpl_vm_init(VM *vm);
void proxpl_vm_free(VM *vm);

/* Interpret a chunk or script: returns InterpretResult */
InterpretResult proxpl_interpret_chunk(VM *vm, const Chunk *chunk);
InterpretResult proxpl_interpret_file(VM *vm, const char *path);

/* High-level helpers */
int proxpl_write_chunk_to_file(const char *path, const Chunk *chunk);
int proxpl_read_chunk_from_file(const char *path, Chunk *out);

#endif /* PROXPL_API_H */
