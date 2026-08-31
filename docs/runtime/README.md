# Runtime Documentation

ProXPL Virtual Machine and runtime execution environment.

## Contents

- [VM Architecture](VM_ARCHITECTURE.md) - Deep-dive into stack-based VM internals
- [Garbage Collection](GARbage_COLLECTION.md) - Mark-and-sweep memory management

## Key Features

- **NaN-Boxing**: Compact 64-bit value encoding
- **Threaded Dispatch**: High-speed instruction execution
- **Call Frames**: Fast function call handling
- **Zero-Cost Exceptions**: ASR resilience unwinding via `setjmp`/`longjmp`

---

**Status**: Stable
