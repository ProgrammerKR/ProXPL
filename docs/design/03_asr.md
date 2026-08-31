# Autonomic Self-Healing (ASR)

Built-in failure recovery with `resilient` and `recovery` blocks.

## Syntax

```proxpl
resilient {
    riskyOperation();
} recovery (e) {
    print("Recovered from: " + e);
}
```

## How It Works

1. Parser reads `resilient` blocks into `STMT_RESILIENT`
2. Bytecode generates `OP_TRY`/`OP_CATCH` equivalent handlers
3. VM uses `setjmp`/`longjmp` for zero-cost exception unwinding

## Status

**IMPLEMENTED** - Full parser, bytecode, and VM support for exception handling. Deep semantic checking and recovery semantics under ongoing stabilization.

---

**Status**: Stable (Beta)
