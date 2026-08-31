# Design Philosophy

ProXPL design principles, architecture decisions, and performance strategy.

## Design Principles

1. **Simplicity**: Clean separation of concerns
2. **Clarity**: Readable, maintainable code
3. **Performance**: Bytecode compilation for speed
4. **Extensibility**: Easy to add features
5. **Debuggability**: Built-in debugging support

## Architecture Decisions

### Stack-Based VM

Chosen over register-based VMs for:
- Simpler implementation
- Minimal memory footprint
- Better debuggability

Trade-off: More instructions for complex operations.

### NaN-Boxing

All values encoded in a single 64-bit value:
- Eliminates tagged unions
- Improves cache locality
- Compact representation

### Multi-Paradigm Design

ProXPL uniquely combines OOP, IOP, and COP:
- **OOP**: Structural organization and code reuse
- **IOP**: Service discovery and behavioral decoupling
- **COP**: Cross-cutting concerns and dynamic adaptation

### SSA-Based Optimization

IR uses Static Single Assignment form for:
- Constant folding
- Dead code elimination
- Common subexpression elimination

## Performance Strategy

1. **Zero-cost abstractions**: High-level features compile to efficient code
2. **SSA optimizations**: Compile-time optimization passes
3. **LLVM backend**: Industry-standard optimizer for native performance
4. **JIT potential**: Foundation for future JIT compilation

## Trade-offs

| Decision | Rationale | Trade-off |
|----------|-----------|-----------|
| Stack-based VM | Simplicity, debuggability | More instructions |
| NaN-boxing | Compact values | Complex type checks |
| SSA IR | Powerful optimizations | Compilation overhead |
| Mark-and-sweep GC | Simplicity | Pause times |
| LLVM AOT | Native performance | LLVM dependency |

---

**Status**: Stable
**See Also**: [VM Architecture](../runtime/VM_ARCHITECTURE.md), [Pillars](../design/PARADIGMS.md)
