# Multi-Paradigm Architecture

ProXPL combines three programming paradigms for comprehensive expression.

## 1. Object-Oriented Programming (OOP)

Class-based inheritance and encapsulation.

```proxpl
class Animal {
    func makeSound() { print("Generic sound") }
}

class Dog extends Animal {
    func makeSound() { print("Woof!") }
}
```

**Features**: Classes, inheritance, interfaces, access modifiers, polymorphism.

## 2. Intent-Oriented Programming (IOP)

Define *what* you want, not *how* to do it.

```proxpl
intent Storage(data: string, path: string);

resolver LocalStorage matches Storage { }
resolver CloudStorage matches Storage { }

// Runtime selects best implementation
Storage("database_backup", "/etc/config");
```

## 3. Context-Oriented Programming (COP)

Dynamic behavioral adaptation based on execution context.

```proxpl
context PerformanceMode {
    layer MathOps {
        func multiply(a, b) { return gpu_mul(a, b); }
    }
}

activate PerformanceMode {
    let x = multiply(10, 20); // Uses PerformanceMode implementation
}
```

## Why Multi-Paradigm?

- **OOP**: Structural organization, code reuse
- **IOP**: Service discovery, behavioral decoupling
- **COP**: Cross-cutting concerns, dynamic adaptation

## The 10 Operational Pillars

| Pillar | Concept | Status |
|--------|---------|--------|
| [Intent-Oriented](01_intent_oriented.md) | `intent`/`resolver` | Stable |
| [Context-Aware](02_context_aware.md) | `context`/`layer`/`activate` | Stable |
| [Autonomic Self-Healing](03_asr.md) | `resilient`/`recovery` | Stable |
| [Intrinsic Security](04_intrinsic_security.md) | Taint analysis | Partial |
| [Chrono-Native](05_chrono_native.md) | Temporal variables | Frontend Only |
| [Distributed Primitives](06_distributed_primitives.md) | Node types | Stub |
| [AI-Native](07_ai_native.md) | Model/Train/Predict | Partial |
| [Quantum-Ready](08_quantum_ready.md) | Qubit/Superpose | Stub |
| [Hardware Math](09_hardware_math.md) | GPU/Kernels | Stub |
| [Zero-Trust](10_zero_trust.md) | Identity/Crypto | Stub |

---

**Status**: Stable (OOP, IOP, COP), Partial/Stub (pillars 4-10)
**See Also**: [Intent-Oriented](01_intent_oriented.md), [Self-Healing ASR](03_asr.md)
