# Compiler Documentation

ProXPL multi-phase compiler implementation and specifications.

## Contents

- [Compiler Architecture](ARCHITECTURE.md) - Pipeline overview and components
- [Bytecode Specification](BYTECODE_SPEC.md) - Instruction set architecture
- [Opcode Reference](OPCODES.md) - Complete opcode mapping
- [Bytecode Format](BYTECODE_FORMAT.md) - Encoding and chunk structure

## Pipeline Overview

```
Source (.prox) → Lexer → Parser (AST) → Type Checker → IR (SSA) → Optimizer → Bytecode Gen → VM / LLVM
```

---

**Status**: Stable
