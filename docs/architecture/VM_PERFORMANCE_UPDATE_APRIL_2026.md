<!--
  Project: ProX Programming Language (ProXPL)
  Author:  ProgrammerKR
  Created: 2026-04-28
  Copyright © 2026. ProXentix India Pvt. Ltd. All rights reserved.
-->

# ProXPL VM Performance Milestone (April 2026)

This document outlines the major architectural optimizations implemented to accelerate the ProXPL Virtual Machine. These changes represent a transition from a baseline interpreter to a high-performance threaded runtime.

## 🚀 Key Optimizations

### 1. Direct Threaded Dispatch Fix
Fixed a critical architectural bug in the experimental threaded dispatchers where the jump table was being rebuilt every iteration.
- **Files**: `src/vm/vm_dispatch.c`, `src/vm/vm_core_opt.c`
- **Improvement**: Jump table is now initialized once before the hot loop.
- **Impact**: Enables true computed-goto performance (~1.5x-2x speedup on GCC/Clang).
- **Safety**: Added `do_INVALID` trap handlers to all 256 opcode slots to prevent undefined behavior on unrecognized bytes.

### 2. Register Caching (ip & stackTop)
The primary execution loop now caches the most volatile pointers in local CPU registers.
- **File**: `src/runtime/vm.c`
- **Details**:
    - `ip` (Instruction Pointer) and `stackTop` are now local `register` variables.
    - Reduced memory traffic by avoiding constant structure dereferencing.
    - Implemented `STORE_FRAME()` and `LOAD_FRAME()` synchronization primitives for C-call safety.

### 3. Global Inline Caching (GIC)
Implemented a single-level inline cache for global variable lookups to bypass hash table overhead.
- **Infrastructure**: Added `cache` field to `ObjFunction` and `GICEntry` structure in `vm.h`.
- **Logic**:
    - On first access, the variable's entry address and table pointer are cached.
    - Subsequent accesses verify the table pointer (to handle rehashes) and perform a direct memory read.
- **Impact**: Turns $O(n)$ hash probes into $O(1)$ direct access.

### 4. Hash Table Arithmetic Optimization
Optimized the core mapping logic in the `Table` implementation.
- **File**: `src/runtime/table.c`
- **Optimization**: Replaced expensive modulo (`%`) with bitwise AND (`&`) for indexing.
- **Prerequisite**: Ensured all table capacities are powers of two (already guaranteed by growth logic).

---

## 📊 Performance Matrix (Estimated)

| Optimization | Target Metric | Expected Gain |
| :--- | :--- | :--- |
| Threaded Dispatch | Instruction Overhead | 30% - 50% |
| Register Caching | Memory Traffic | 15% - 20% |
| Inline Caching | Global Variable Speed | 200% - 400% |
| Bitwise Indexing | Table Lookup Latency | 5% - 10% |

## 🛠️ Internal Changes for Developers

- **`ObjFunction`**: Now includes a `void* cache` which is GC-managed (freed in `gc.c`).
- **`Table`**: New `tableGetEntry()` function provides direct `Entry*` access for caching systems.
- **`VM` Loop**: All opcode handlers in `run()` must now use local `ip` and `stackTop` and call `STORE_FRAME()` before any operation that might trigger a GC or use the VM's global state.

---
*Status: Milestone 1 Complete*
