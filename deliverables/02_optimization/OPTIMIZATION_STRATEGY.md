# ProXPL Performance Optimization Strategy (phased)

Prepared: 2025-12-12

Overview
- Goal: transform ProXPL interpreter into a high-performance runtime with production-grade tooling.
- Approach: phased milestones from low-risk interpreter optimizations → efficient runtime → JIT integration → ecosystem.

Milestones
1. Hotspot Analysis & Microbenchmarks (2 weeks)
   - Build benchmark harness targeting arithmetic, calls, string ops, allocation.
   - Collect per-op timings, branch misprediction, cache miss rates.
2. Bytecode & Dispatch Redesign (3 weeks)
   - Move to register-friendly bytecode where appropriate.
   - Use compact operand encodings and operand folding for common cases.
   - Implement computed-goto dispatch for supported compilers and a hot-path inline dispatch for Windows/MSVC.
3. Inline Caching & Type Specialization (4 weeks)
   - Add monomorphic/polymorphic inline caches for property access and call targets.
   - Specialize arithmetic ops by observed operand types and emit fast paths.
4. Memory Management & GC (4-6 weeks)
   - Implement a generational, moving GC (semi-space or copying nursery) + incremental major collector.
   - Add write barriers and object header tagging for fast type checks.
5. JIT Compiler (LLVM-backed) (8-12 weeks)
   - Lower hot traces or functions to a compact IR, apply type-specialization, and emit native code via LLVM ORC JIT.
6. Tooling & Ecosystem (ongoing)
   - LSP, VSCode extension, package manager improvements, CI/CD, docs, examples.

Examples and small designs are in `src/vm/vm_core_opt.c` and `deliverables/03_jit/JIT_DESIGN.md`.
