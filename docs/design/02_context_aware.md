# Context-Aware Polymorphism

Dynamic behavioral adaptation based on execution context.

## Syntax

```proxpl
context PerformanceMode {
    layer MathOps {
        func multiply(a, b) {
            return gpu_mul(a, b); // High-performance GPU impl
        }
    }
}

activate PerformanceMode {
    let x = multiply(10, 20); // Uses PerformanceMode implementation
}
```

## Components

- **Context**: Top-level container for behavioral variations
- **Layer**: Collection of method overrides active when context is enabled
- **Activate**: Block to apply layers to a scope

## Status

**IMPLEMENTED** - Parser supports them. `bytecode_gen.c` emits `OP_CONTEXT`, `OP_LAYER`, and `OP_ACTIVATE`. VM executes these opcodes and manages an active context stack. Type Checker verifies `STMT_CONTEXT_DECL` and `STMT_LAYER_DECL`.

---

**Status**: Stable
