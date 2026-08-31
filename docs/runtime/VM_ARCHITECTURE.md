# ProXPL Virtual Machine Architecture

Technical deep-dive into the stack-based VM.

## Overview

ProXPL's VM is a high-performance stack-based bytecode interpreter featuring NaN-boxed values and mark-and-sweep garbage collection.

## Key Features

- **NaN-Boxing**: Compact 64-bit value representation
- **Threaded dispatch**: Computed goto for optimal execution
- **Mark-and-sweep GC**: Automatic memory management
- **Call frames**: Efficient function calls
- **45+ instructions**: Arithmetic, control flow, OOP, COP

## Value Representation

NaN-boxing uses IEEE 754 quiet NaN payload space to encode all types in a single `uint64_t`:

```c
#define QNAN ((uint64_t)0x7ffc000000000000)
#define SIGN_BIT ((uint64_t)0x8000000000000000)
```

Types encoded in NaN space:
- Doubles (standard IEEE 754)
- Pointers (in NaN space)
- Booleans and small integers (tagged)

## Stack-Based Execution

```
┌──────────────────────────────────┐
│ Stack (grows upward)            │
│ [sp] → top                     │
│ ...                             │
│ [stack[0]] (bottom)            │
└──────────────────────────────────┘
```

## Call Frames

```c
typedef struct {
    ObjFunction *function;
    uint8_t *ip;              // Instruction pointer
    Value *slots;             // Local variables
} CallFrame;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    int frameCount;
} VM;
```

## Execution Loop

```c
for (;;) {
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
        case OP_CONSTANT:
            push(vm, vm->chunk->constants.values[READ_BYTE()]);
            break;
        // ... more cases
        case OP_RETURN:
            return INTERPRET_OK;
    }
}
```

## Garbage Collection

Mark-and-sweep triggered when allocated memory exceeds threshold:
1. **Mark**: Start from roots, follow all references
2. **Sweep**: Free unmarked objects
3. **Threshold**: Doubles after each collection

## Performance Optimizations

- String interning for O(1) comparison
- Constant pool for shared literals
- Efficient NaN-boxed value representation
- Threaded dispatch (computed goto)

## Safety Guarantees

- Type checking at compile time
- Bounds-checked array access
- Null safety via Option types
- Structured exception handling (ASR)

---

**Status**: Stable
**See Also**: [Compiler Architecture](../compiler/ARCHITECTURE.md), [Bytecode Spec](../compiler/BYTECODE_SPEC.md), [GC](GC.md)
