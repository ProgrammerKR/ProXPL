# ProXPL Opcodes

Canonical opcode set for the ProXPL bytecode VM. Source of truth: `include/bytecode.h`.

| Mnemonic | Byte | Description |
|----------|------|-------------|
| `OP_NOP` | 0x00 | No operation |
| `OP_ADD` | 0x01 | Numeric addition |
| `OP_SUB` | 0x02 | Numeric subtraction |
| `OP_MUL` | 0x03 | Numeric multiplication |
| `OP_DIV` | 0x04 | Numeric division |
| `OP_MOD` | 0x05 | Numeric modulo |
| `OP_NEG` | 0x06 | Numeric negate |
| `OP_EQ` | 0x10 | Compare equal |
| `OP_NEQ` | 0x11 | Not equal |
| `OP_LT` | 0x12 | Less than |
| `OP_LTE` | 0x13 | Less than or equal |
| `OP_GT` | 0x14 | Greater than |
| `OP_GTE` | 0x15 | Greater than or equal |
| `OP_NOT` | 0x20 | Logical not |
| `OP_AND` | 0x21 | Logical AND |
| `OP_OR` | 0x22 | Logical OR |
| `OP_PUSH_CONST` | 0x30 | Push constant from table |
| `OP_LOAD_REG` | 0x31 | Load from register |
| `OP_STORE_REG` | 0x32 | Store into register |
| `OP_POP` | 0x33 | Pop from stack |
| `OP_DUP` | 0x34 | Duplicate top of stack |
| `OP_JMP` | 0x40 | Unconditional jump |
| `OP_JMP_IF_TRUE` | 0x41 | Jump if true |
| `OP_JMP_IF_FALSE` | 0x42 | Jump if false |
| `OP_CALL` | 0x50 | Call function |
| `OP_RETURN` | 0x51 | Return from function |
| `OP_TAIL_CALL` | 0x52 | Tail-call optimization |
| `OP_MAKE_FUNCTION` | 0x60 | Create function proto |
| `OP_CLOSURE` | 0x61 | Create closure |
| `OP_CLOSE_UPVALUE` | 0x62 | Close upvalue |
| `OP_LOAD_UPVALUE` | 0x63 | Load upvalue |
| `OP_STORE_UPVALUE` | 0x64 | Store into upvalue |
| `OP_NEW_ARRAY` | 0x70 | Create array |
| `OP_INDEX_GET` | 0x71 | Indexing read |
| `OP_INDEX_SET` | 0x72 | Indexing write |
| `OP_GET_FIELD` | 0x73 | Get object field |
| `OP_SET_FIELD` | 0x74 | Set object field |
| `OP_DBG_LINE` | 0x90 | Debug line info |
| `OP_DBG_LOC` | 0x91 | Debug location |
| `OP_HALT` | 0xFF | Halt VM |

## Extension Points

- JIT/introspection: expose deterministic execution traces
- Fast path for numerics and strings (tagged values)

## Backwards Compatibility

- Keep opcode order stable; additions should not reuse values
- Removed opcodes: mark deprecated, reserve numeric value

---

**Status**: Stable
**See Also**: [Bytecode Spec](BYTECODE_SPEC.md), [VM Architecture](../runtime/VM_ARCHITECTURE.md)
