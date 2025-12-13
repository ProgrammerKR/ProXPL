# TICKET-002: Bytecode Emitter and Compiler Improvements

Goal
----
Complete the front-end bytecode emitter to support full language constructs and ensure it outputs valid chunks for the VM.

Acceptance Criteria
-------------------
- All AST constructs are compiled into bytecode and pass round-trip tests.
- Add unit tests for arithmetic, functions, closures, and control flow emitting bytecode and disassembling.

Notes
-----
- Compiler should use `include/bytecode.h` as source for opcodes.
