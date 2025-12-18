# ProXPL V2 Architecture: High-Performance Runtime

## 1. Executive Summary
This document outlines the architectural transformation of ProXPL from a tree-walking interpreter to a high-performance JIT-compiled runtime.
**Goal:** Achieve performance parity with LuaJIT/V8 and >3x speedup over CPython 3.11.

## 2. Core Components

### 2.1 Compiler (Frontend)
Separated from the runtime, responsible for:
- **Lexing/Parsing**: Producing an AST.
- **Bytecode Generation**: Traversing the AST and emitting Register Machine IR.
- **Optimization**: Constant folding, dead code elimination.

### 2.2 Register-Based Virtual Machine (Runtime)
A register VM reduces instruction dispatch overhead compared to stack-based VMs.

**Instruction Set Architecture (ISA)**
Fixed-width 32-bit instructions for cache locality.
Format: `[ OpCode (8) | A (8) | B (8) | C (8) ]`

| Field | Bits | Purpose |
|-------|------|---------|
| OpCode| 0-7  | Operation identifier |
| A     | 8-15 | Destination Register / Operand |
| B     | 16-23| Source Register 1 |
| C     | 24-31| Source Register 2 / Immediate |

**Core Instructions:**
- `LOAD_CONST R(A), K(Bx)`: Load constant from pool at index Bx into R(A).
- `MOV R(A), R(B)`: Move value from R(B) to R(A).
- `ADD R(A), R(B), R(C)`: R(A) = R(B) + R(C) (Specialized for Int/Float).
- `CALL R(A), R(B), C`: Call function at R(A) with args starting at R(B), count C.
- `RETURN R(A)`: Return value in R(A).
- `JMP_IF R(A), Offset(sBx)`: Conditional Jump.

### 2.3 Memory Model (NaN-Boxing)
All values are 64-bit doubles.
- **Doubles**: Standard IEEE 754 doubles.
- **Pointers/Tags**: Stored in the NaN space (top 16 bits).
   - `0xFFF8` prefix indicates NaN.
   - Low 48 bits are pointers or immediate values (Bool, Null, Int32).

**Layout:**
```
[ 111111111111 0000 | ... payload ... ] -> Double NaN
[ 111111111111 0001 | ... address ... ] -> Object Pointer
[ 111111111111 0010 | ... integer ... ] -> 32-bit Integer
[ 111111111111 0011 | 0/1             ] -> Boolean
```

### 2.4 Garbage Collection
- **Generational GC**:
    - **Nursery**: Bump-pointer allocation (extremely fast). Logic: `ptr = top; top += size;`
    - **Old Gen**: Mark-and-Sweep.
    - **Write Barriers**: Required when Old Gen object points to Young Gen.

### 2.5 JIT Compilation Strategy (Baseline)
- **Template JIT**:
    - Pre-compile assembly snippets for each bytecode op.
    - At JIT time ($$ > 100 runs $$), `memcpy` snippets into a specialized buffer.
    - Patch jumps and immediate values.
- **Deoptimization**: Support bailing out to interpreter if assumptions fail (e.g., type check failure in optimized code - primarily for later tracing JIT, but relevant for simple type guards).

## 3. Project Structure Refactor
```
src/
├── compiler/       # Frontend (Parser, AST, CodeGen)
│   ├── lexer/
│   ├── parser/
│   └── codegen/
├── vm/             # Runtime (VM, GC, Loader)
│   ├── core/       # Loop, Dispatch
│   ├── gc/         # Allocator, Collector
│   └── jit/        # Baseline JIT
├── stdlib/         # Native implementations
└── include/        # Public headers
```
