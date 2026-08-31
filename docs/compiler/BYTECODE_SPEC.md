# Bytecode Specification

ProXPL VM instruction set architecture.

## Instruction Format

Each instruction is 1-3 bytes:
```
[OpCode: 1 byte] [Operand: 0-2 bytes]
```

Addressing modes use ULEB128 for compact encoding.

## Opcodes

### Stack Manipulation
- `OP_CONSTANT` - Push constant from constant pool
- `OP_PUSH_CONST` - Push constant (index immediate)
- `OP_LOAD_REG` - Load from register by index
- `OP_STORE_REG` - Store into register
- `OP_POP` - Pop from stack
- `OP_DUP` - Duplicate top of stack
- `OP_NIL`, `OP_TRUE`, `OP_FALSE` - Push constants

### Arithmetic/Logic
- `OP_ADD`, `OP_SUB`, `OP_MUL`, `OP_DIV`, `OP_MOD`
- `OP_NEG`, `OP_NOT`, `OP_BITWISE_NOT`
- `OP_AND`, `OP_OR`, `OP_BITWISE_AND`, `OP_BITWISE_OR`, `OP_BITWISE_XOR`
- `OP_LEFT_SHIFT`, `OP_RIGHT_SHIFT`

### Comparison
- `OP_EQ`, `OP_NEQ`, `OP_LT`, `OP_LTE`, `OP_GT`, `OP_GTE`

### Control Flow
- `OP_JMP` - Unconditional jump
- `OP_JMP_IF_TRUE`, `OP_JMP_IF_FALSE` - Conditional jumps

### Functions
- `OP_CALL`, `OP_TAIL_CALL`, `OP_RETURN`
- `OP_MAKE_FUNCTION`, `OP_CLOSURE`
- `OP_LOAD_UPVALUE`, `OP_STORE_UPVALUE`, `OP_CLOSE_UPVALUE`

### Collections and Objects
- `OP_NEW_ARRAY`, `OP_INDEX_GET`, `OP_INDEX_SET`
- `OP_GET_FIELD`, `OP_SET_FIELD`

### Debug
- `OP_DBG_LINE`, `OP_DBG_LOC` - Debug metadata

### Special
- `OP_HALT` - Halt VM (for tests)
- `OP_CONTEXT`, `OP_LAYER`, `OP_ACTIVATE` - COP opcodes
- `OP_TRY`, `OP_CATCH` - ASR exception handling

---

**Status**: Stable
**See Also**: [Opcodes](OPCODES.md), [VM Architecture](../runtime/VM_ARCHITECTURE.md)
