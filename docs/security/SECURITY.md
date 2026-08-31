# Security Model

ProXPL security architecture.

## Language-Level Security

### Intrinsic Security (IFC)
- **Taint Tracking**: Type-level taint flags (`isTainted`)
- **Sanitize Primitives**: Built-in `sanitize()` function
- **Status**: Partially implemented (static analysis only)

### Memory Safety
- Mark-and-sweep garbage collection
- No manual memory management for user code
- String interning prevents buffer issues

### Sandboxing
- FFI requires explicit `extern` declarations
- Module imports follow strict resolution rules

## Runtime Security

### Exception Handling
- Structured `try`/`catch`/`finally`
- ASR `resilient`/`recovery` blocks
- Stack traces with file/line info

### Future (Planned)
- WebAssembly sandboxing (v1.6.3+)
- Capabilities-based security
- Zero-trust identity verification blocks

---

**Status**: Partial (language-level), Planned (runtime-level)
**See Also**: [Intrinsic Security](../design/04_intrinsic_security.md), [Zero-Trust](../design/10_zero_trust.md)
