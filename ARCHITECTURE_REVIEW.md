# ProXPL Architecture Review

Date: 2025-12-13

Overview
--------
This document provides a high-level review of the ProXPL repository and architectural recommendations focused on modernizing, modularizing, and preparing the codebase for a production-grade VM and ecosystem.

Current Layout
--------------
- Top-level: `src/` contains the primary C implementation (lexer, parser, runtime, stdlib).
- Public headers: `include/` exposes the VM and compiler APIs.
- Tests: `tests/` covers bytecode and scanner tests.
- Docs: `docs/` and `ARCHITECTURE.md` contain comprehensive design and language details.

Key Observations
----------------
- The code is separated into front-end (lexer/parser/type checker) and runtime (compiler, vm, chunk, intrinsics), which is a good foundation.
- VM implementation is a working stack-based interpreter with bytecode and a simple chunk system; opcode definitions are centralized in `include/bytecode.h`.
- There are separate `src/runtime` and `src/vm` directories; unify or clearly separate their responsibilities (runtime = primtiive runtime helpers, vm = dispatch & optimizations).
- Missing/partial features: GC, IR, optimizer, JIT, module system, FFI, concurrency, standard library modules, tooling.

Risks & Architectural Trade-offs
--------------------------------
- Monolithic vs Modular: The current structure scales slowly (many top-level sources with cross-cutting includes). A clearer modular split (frontend/compiler/vm/runtime/stdlib/tools) will improve maintainability.
- Versioning & ABI: As this project evolves, design public stable headers and internal headers; add `include/proxpl_api.h` that documents stable APIs.
- GC/Heap: The current memory model is simple; target runtime should implement a generational GC with write barriers for performance and correctness in concurrency.

Recommended Modular Structure
-----------------------------
- `frontend/` — lexer, parser, type-checker, AST, semantic analysis.
- `compiler/` — IR, optimizations, bytecode emitter, linking.
- `vm/` — bytecode format, bytecode interpreter, debugger hooks.
- `runtime/` — memory management, GC, object model, compact object representation, GC safety wrappers.
- `stdlib/` — native modules and bindings.
- `tools/` — bench, build scripts, LSP, REPL utilities.
- `ffi/` — C/Python bridge, FFI design and wrappers.

Short-term Action Items (Phase 0)
--------------------------------
1. Add a `TICKET-001: ir/opcodes.md` spec from `include/bytecode.h` and commit as canonical spec.
2. Add `include/proxpl_api.h` and define the public stable API for VM and runtime.
3. Create `scripts/` with `build_unix.sh` and `build_windows.ps1` that use CMake for cross-platform builds; add `CI` workflows to GitHub Actions.
4. Move test harness to `tests/` and provide a `tests/CMakeLists.txt` to allow running tests with `ctest`.
5. Add a `tools/bench` harness that executes core micro-benchmarks and produces a simple JSON report.

Longer-term Priorities
----------------------
- Implement generational GC with write barriers and a memory profiler.
- Implement the IR and an optimizer pipeline with canonical IR passes (constant folding, DCE, inlining, escape analysis).
- Add a JIT backend (start with a simple tracing JIT prototype or LLVM). Provide hooks in the VM for hot-trace extraction.
- Add concurrency primitives: coroutines, channels, and threads backed by a scheduler.
- Implement FFI for C and Python (MLVM-style embedding) with security hardening.
- Ship an LSP server + VSCode extension with DAP instrumentation for the VM.

Acceptance Criteria / Metrics
---------------------------
- The repo should build via CMake on Linux/macOS/Windows using standard toolchains (GCC/Clang/MSVC).
- The VM should provide consistent API across versions; header stability maintained via `include/proxpl_api.h`.
- Micro-bench suite should show a reproducible baseline for CPU-bound and IO-bound tests.

Next Steps I'm Prepared To Implement
-----------------------------------
- Add `ir/opcodes.md` and a clean, canonical opcode spec.
- Add CMake test configuration and a sample GitHub Actions workflow.
- Add `scripts/build_*.sh/.ps1` build helpers and a `tools/bench` harness.
