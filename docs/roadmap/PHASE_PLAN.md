# ProXPL Modernization Phase Plan

This document consolidates the modernization roadmap into phases aligned with the project's goals.

Phase A - Core Runtime
- Design IR and opcode set (docs/ir/opcodes.md)
- Implement complete bytecode emitter and stack-based VM
- Add microbenchmarks and perf harness

Phase B - Memory Model & Concurrency
- Implement generational garbage collector with write barriers
- Add coroutines, async/await, threads, channels, actor model
- Provide scheduler (cooperative + preemptive)

Phase C - Optional Static Type System
- Optional type annotations and type inference
- Generics and specialization
- Type-based optimizations and VM fast paths

Phase D - Standard Library
- Filesystem, networking, crypto, JSON, regex, datetime, subprocess
- Database connectors via FFI

Phase E - Tooling & Ecosystem
- Package manager (prx), registry service
- LSP server, Debugger (DAP), VSCode extension
- REPL improvements

Phase F - JIT Backend
- Tracing JIT / LLVM-based backend
- Hot-path detection and codegen
- JIT > 2x baseline performance target

Testing and CI
- Unit coverage target 95%
- Fuzzing for parser/VM
- Cross-platform CI matrix (Linux/macOS/Windows)

Security & Migration
- FFI sandboxing and capability-based modules
- Migration guides & codemods
