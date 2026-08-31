# Autonomic Self-Healing (ASR) Runtime

Experimental runtime for resilient execution with automatic recovery.

## Overview

ASR provides built-in failure recovery with `resilient` and `recovery` blocks. The runtime uses `setjmp`/`longjmp` for zero-cost exception unwinding.

## Syntax

```proxpl
resilient {
    // Code that might fail
    riskyOperation();
} recovery (e) {
    // Recovery handler
    print("Recovered from: " + e);
}
```

## Implementation Status

**Status**: Partial - The Parser reads `resilient` blocks and Bytecode generates `OP_TRY`/`OP_CATCH` equivalent handlers. The VM uses `setjmp`/`longjmp` for exception unwinding. However, deep semantic checking and recovery semantics are still being stabilized.

## Components

1. **Parser**: Generates `STMT_RESILIENT` nodes
2. **Bytecode**: Emits exception handling opcodes
3. **VM**: `setjmp`/`longjmp` for unwinding
4. **Type Checker**: Validates resilient/recovery block structure

## Limitations

- Exception semantics not fully stabilized
- Recovery value propagation needs verification
- Stack unwinding edge cases under investigation

---

**Status**: Beta/Experimental
**See Also**: [Pillars](03_asr.md)
