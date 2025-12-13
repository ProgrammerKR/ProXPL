# TICKET-001: Define opcode spec

Background
----------
We need a canonical opcode list for the ProXPL bytecode VM. This should be the source of truth for serialization formats, VM dispatch, JIT, and disassembler.

Deliverables
------------
1. `docs/ir/opcodes.md` (canonical human-readable spec).
2. `include/bytecode.h` to be kept in sync with the spec and as single source of truth for code.
3. Unit tests that parse and disassemble example bytecode.

Acceptance Criteria
-------------------
- `docs/ir/opcodes.md` exists and matches the enum in `include/bytecode.h`.
- Disassembly test (`tests/bytecode_tests.c`) verifies opcodes round-trip with write/read.

Notes
-----
- This ticket is a prerequisite for `TICKET-002: Implement bytecode emitter`.
