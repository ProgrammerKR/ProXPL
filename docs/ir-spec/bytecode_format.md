# ProXPL Bytecode Format Specification

## Overview
ProXPL uses a stack-based bytecode format. Instructions are 1-byte opcodes followed by variable-length arguments.

## Instruction Set

| Opcode | Name | Arguments | Description |
|--------|------|-----------|-------------|
| 0x01 | ADD | - | Popp 2 values, add, push result |
| 0x02 | SUB | - | Pop 2 values, sub, push result |
| 0x03 | MUL | - | Pop 2 values, mul, push result |
| 0x04 | DIV | - | Pop 2 values, div, push result |
| 0x10 | LOAD | index | Push local variable[index] |
| 0x11 | STORE | index | Pop value, store in local[index] |
| 0x20 | JUMP | label | Unconditional jump |
| 0x21 | JUMP_IF | label | Pop cond, jump if true |
| 0x30 | CALL | func_id | Call function |
| 0x31 | RET | - | Return from function |

## File Header
- Magic: `PROX`
- Version: `0x01`
- Checksum: `CRC32`
