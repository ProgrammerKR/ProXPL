# ProXPL Opcode Specification

This document captures the canonical opcode set for the ProXPL bytecode VM. The source-of-truth is the `include/bytecode.h` header — keep this doc in sync with the header enum.

Format
------
- Mnemonic: Byte value — Description

Opcodes
-------
- OP_NOP: 0x00 — No operation.
- OP_ADD: 0x01 — Numeric addition (pop two numbers, push result).
- OP_SUB: 0x02 — Numeric subtraction.
- OP_MUL: 0x03 — Numeric multiplication.
- OP_DIV: 0x04 — Numeric division.
- OP_MOD: 0x05 — Numeric modulo.
- OP_NEG: 0x06 — Numeric negate.

- OP_EQ: 0x10 — Compare equal.
- OP_NEQ: 0x11 — Not-equal.
- OP_LT: 0x12 — Less-than.
- OP_LTE: 0x13 — Less-than-or-equal.
- OP_GT: 0x14 — Greater-than.
- OP_GTE: 0x15 — Greater-than-or-equal.

- OP_NOT: 0x20 — Logical not.
- OP_AND: 0x21 — Logical AND (short-circuit handled by compiler via jumps).
- OP_OR: 0x22 — Logical OR.

- OP_PUSH_CONST: 0x30 — Push a constant from constant table (index immediate).
- OP_LOAD_REG: 0x31 — Load from register (or local slot) by index.
- OP_STORE_REG: 0x32 — Store into register/local slot by index.
- OP_POP: 0x33 — Pop value from stack.
- OP_DUP: 0x34 — Duplicate top of stack.

- OP_JMP: 0x40 — Unconditional relative jump (signed offset).
- OP_JMP_IF_TRUE: 0x41 — Jump if top-of-stack is true (pops or peeks depending on convention).
- OP_JMP_IF_FALSE: 0x42 — Jump if top-of-stack is false.

- OP_CALL: 0x50 — Call function (arity immediate or stack-based call).
- OP_RETURN: 0x51 — Return from a function (possibly with value on the stack).
- OP_TAIL_CALL: 0x52 — Tail-call optimization variant.

- OP_MAKE_FUNCTION: 0x60 — Create a new function proto object and push it.
- OP_CLOSURE: 0x61 — Create a closure with captured upvalues.
- OP_CLOSE_UPVALUE: 0x62 — Close an open upvalue.
- OP_LOAD_UPVALUE: 0x63 — Load from upvalue to stack.
- OP_STORE_UPVALUE: 0x64 — Store into upvalue.

- OP_NEW_ARRAY: 0x70 — Create a new array (arity immediate or pop-based args).
- OP_INDEX_GET: 0x71 — Indexing read (array/dict), stack: (container, index) → push value.
- OP_INDEX_SET: 0x72 — Indexing write (container, index, value).
- OP_GET_FIELD: 0x73 — Get object field by name (index immediate or constant string).
- OP_SET_FIELD: 0x74 — Set object field by name.

- OP_DBG_LINE: 0x90 — Debug line metadata.
- OP_DBG_LOC: 0x91 — Debug location metadata.

- OP_HALT: 0xFF — Halt VM (good for tests)

Addressing Modes & Encodings
----------------------------
- Encode immediates with ULEB128 for compactness (small integers) for indices and offsets.
- For calls, allow either an arity immediate or pass arity via a prior OP_PUSH_CONST.

Extension Points
----------------
- For JIT/introspection, expose a deterministic trace/log of bytecode execution that can be fed into a trace compiler.
- Add a fast path for numerics and strings (SMI-ish, tagged values) — allow the JIT to specialize on types.

Backwards Compatibility
-----------------------
- Keep order in the opcode enum stable; adding new opcodes should avoid reusing values.
- When removing opcodes, mark them as deprecated and keep their numeric values reserved.
