/* bench_simple.c
 * A tiny microbenchmark that constructs a numeric-add heavy Chunk and
 * executes it repeatedly to measure interpreter throughput.
 * This file expects the project to expose Chunk/bytecode helpers and
 * a vm_run_chunk_simple() entry in `src/vm/vm_dispatch.c`.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/bytecode.h"

extern int vm_run_chunk_simple(const Chunk *chunk);

#if defined(_WIN32)
#include <windows.h>
static double now_seconds(void) {
    static LARGE_INTEGER freq;
    LARGE_INTEGER cnt;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart / (double)freq.QuadPart;
}
#else
static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}
#endif

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    /* Build a simple chunk: push 100 constants then add them pairwise */
    Chunk c; initChunk(&c);
    for (int i = 0; i < 100; ++i) {
        emit_constant(&c, NUMBER_VAL((double)i));
    }
    /* Create add sequence */
    for (int i = 0; i < 99; ++i) emit_opcode(&c, OP_ADD);
    emit_opcode(&c, OP_HALT);

    int runs = 500;
    double t0 = now_seconds();
    for (int i = 0; i < runs; ++i) {
        int r = vm_run_chunk_simple(&c);
        if (r != 0) { fprintf(stderr, "vm returned %d\n", r); break; }
    }
    double t1 = now_seconds();
    printf("runs=%d total=%.6fs avg=%.6fms\n", runs, t1-t0, (t1-t0)/runs*1000.0);

    freeChunk(&c);
    return 0;
}
