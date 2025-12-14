# ProXPL Performance Modernization Design Doc

## 1. System Architecture (Target State)

```text
       +------------------+
       |   Source Code    | (.prox)
       +--------+---------+
                |
       +--------v---------+
       |   Lexer/Parser   | (C / Modernized)
       +--------+---------+
                |
       +--------v---------+
       |    AST Builder   |
       +--------+---------+
                |
       +--------v---------+
       |  Bytecode Compiler| (AST -> REG-ISA)
       | (Reg Allocator)  |
       +--------+---------+
                |
       +--------v---------+
       |   Bytecode Module|<-----+  Native Runtime   |
       |  (Instructions)  |      | (C/Rust Strings,  |
       +--------+---------+      |  Arrays, Dicts)   |
                |                +-------------------+
    +-----------v-----------+
    |     Register VM       | <--- Profiling Events
    | (Interpreter Loop)    |
    +-----------+-----------+
                |
          [Hot Path?]
                |
    +-----------v-----------+
    |   Baseline JIT (C)    | (Template/Copying JIT)
    |  (Machine Code Gen)   |
    +-----------+-----------+
                |
          [Very Hot?]
                |
    +-----------v-----------+     +------------------+
    |   Optimizer (LLVM)    |     |  Inline Caches   |
    | (Type Specialization) |<----+ (Polymorphic IC) |
    +-----------------------+     +------------------+
```

## 2. Design Doc Outline & Modules

### 2.1 Register-Based VM ISA
**Motivation**: Reduce dispatch overhead (fewer instructions than stack VM) and improve cache locality.
**Structure**:
- `Instruction`: 32-bit word.
- `Opcode`: 8 bits.
- `A` (Dest): 8 bits.
- `B` (Src1): 8 bits.
- `C` (Src2/Imm): 8 bits.

**Core Instructions**:
- `MOV R_dest, R_src`
- `ADD R_dest, R_src1, R_src2`
- `LOADK R_dest, K_idx`
- `CALL R_dest, R_func, NumArgs`
- `RET R_src`

### 2.2 Baseline JIT (Template JIT)
**Strategy**:
- Pre-compile machine code snippets for each opcode (templates).
- **Benefit**: Very fast implementation, 2-5x speedup over interpreter.

### 2.3 Optimizing JIT (LLVM / DynASM)
**Strategy**:
- Triggered for hot loops (>10k executions).
- **Type Specialization**: Guard checks for types.

### 2.4 Data Model & Memory Layout
- **Value**: NaN-boxing (64-bit).
- **GC**: Generational Mark-and-Sweep.

## 3. Risks & Tradeoffs
1.  **Complexity**: LLVM is heavy. **Mitigation**: Start with Template JIT.
2.  **GC Pauses**: Generational GC adds complexity. **Mitigation**: Incremental marking.

## 4. Benchmark Plan

**Microbenchmarks**:
1.  `arith_loop.prox`: Tight loop summing integers.
2.  `call_depth.prox`: Recursive fibonacci.
3.  `str_cat.prox`: String concatenation.
