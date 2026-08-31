# ProXPL Compiler Architecture

The multi-phase compiler transforms source code into executable bytecode.

## Pipeline

```
Source Code → Lexer → Parser → Type Checker → IR Optimizer → Bytecode/LLVM
```

## Phases

### 1. Lexical Analysis (Lexer)

**File**: `src/lexer/scanner.c`

Tokenizes source code into lexical tokens. Identifies keywords, operators, literals, and structural elements.

### 2. Parsing (Parser)

**File**: `src/parser/parser.c`

Recursive descent parser builds an Abstract Syntax Tree (AST) from tokens. Uses LL(1) with operator precedence climbing.

### 3. Semantic Analysis (Type Checker)

**File**: `src/compiler/type_checker.c`

Validates scope, type compatibility, and semantic rules. Performs type inference using Hindley-Milner style algorithm.

### 4. Intermediate Representation

**Files**: `src/compiler/ir_gen.c`, `src/compiler/ir_opt.c`

AST is lowered to SSA-based IR. Optimization passes include constant folding and dead code elimination.

### 5. Code Generation

**Files**: `src/compiler/bytecode_gen.c`, `src/compiler/backend_llvm.cpp`

- **Bytecode Path**: Emits stack-based VM bytecode
- **LLVM Path**: Generates LLVM IR for AOT native compilation

### 6. Execution

The stack-based VM executes bytecode directly, or native binaries execute LLVM-compiled code.

## Compiler Components

| Component | Location | Responsibility |
|-----------|----------|----------------|
| Lexer | `src/lexer/scanner.c` | Tokenize source |
| Parser | `src/parser/parser.c` | Build AST |
| Type Checker | `src/compiler/type_checker.c` | Semantic validation |
| IR Generator | `src/compiler/ir_gen.c` | SSA IR generation |
| IR Optimizer | `src/compiler/ir_opt.c` | IR optimizations |
| Bytecode Gen | `src/compiler/bytecode_gen.c` | VM bytecode |
| LLVM Backend | `src/compiler/backend_llvm.cpp` | Native compilation |

## Extension Points

Adding new features requires:
1. Add token type and lexer recognition
2. Add AST node and parser rule
3. Add type checking rules
4. Add bytecode emission
5. Add VM execution support

---

**Status**: Stable
**See Also**: [Bytecode Spec](BYTECODE_SPEC.md), [Opcodes](OPCODES.md), [VM Architecture](../runtime/VM_ARCHITECTURE.md)
