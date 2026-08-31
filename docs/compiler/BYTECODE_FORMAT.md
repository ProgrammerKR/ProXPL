# Bytecode Format

Detailed bytecode encoding specification.

## Chunk Structure

```c
typedef struct {
    uint8_t *code;           // Bytecode instructions
    int *lines;              // Line numbers for each instruction
    ValueArray constants;    // Constant pool
    int capacity;
    int count;
} Chunk;
```

## Instruction Encoding

Each instruction is encoded as:
```
[OpCode: 1 byte] [Operand: 0-2 bytes]
```

### Examples
- `OP_CONSTANT <index>` - 1-2 bytes
- `OP_ADD` - 1 byte
- `OP_JUMP <offset>` - 1-2 bytes
- `OP_CALL <arg_count>` - 1-2 bytes

## Constant Pool

The constant pool stores:
- Numeric literals (int, float)
- String literals
- Function objects
- Class definitions

---

**Status**: Stable
**See Also**: [Opcodes](OPCODES.md), [Bytecode Spec](BYTECODE_SPEC.md)
