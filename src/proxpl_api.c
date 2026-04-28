// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/* proxpl_api.c - public API wrappers for stable ABI
 * Exposes a minimal C-style API that callers can use to embed runtime.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/proxpl_api.h"
#include "../include/vm.h"
#include "../include/bytecode.h"
#include "../include/common.h"
#include "../include/file_utils.h"

void proxpl_vm_init(VM *pvm) {
    if (pvm == NULL) return;
    // Note: ProXPL maintains local VM state in the provided pointer.
    initVM(pvm); 
}

void proxpl_vm_free(VM *pvm) {
    if (pvm == NULL) return;
    freeVM(pvm);
}

InterpretResult proxpl_interpret_chunk(VM *pvm, const Chunk *chunk) {
    (void)pvm; (void)chunk;
    // Interpreting raw chunks requires bypassing the compiler pipeline.
    // This is currently unsupported as it skips essential optimization and safety passes.
    fprintf(stderr, "API Error: interpreting raw chunks is not supported in this version.\n");
    return INTERPRET_RUNTIME_ERROR;
}

InterpretResult proxpl_interpret_file(VM *pvm, const char *path) {
    if (pvm == NULL || path == NULL) return INTERPRET_RUNTIME_ERROR;
    char* source = readFile(path);
    if (source == NULL) return INTERPRET_COMPILE_ERROR;

    // Pass the source string to interpret
    InterpretResult result = interpret(pvm, source);
    
    trackSource(pvm, source);
    return result;
}

// These functions are STUBBED because bytecode.c was deleted.
// You must re-implement binary serialization later if you need it.
int proxpl_write_chunk_to_file(const char *path, const Chunk *chunk) {
    (void)path; (void)chunk;
    fprintf(stderr, "Feature 'write_chunk_to_file' is temporarily disabled.\n");
    return -1;
}

int proxpl_read_chunk_from_file(const char *path, Chunk *out) {
    (void)path; (void)out;
    fprintf(stderr, "Feature 'read_chunk_from_file' is temporarily disabled.\n");
    return -1;
}

