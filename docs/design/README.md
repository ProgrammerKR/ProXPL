# Design & Architecture

ProXPL design philosophy, paradigms, and experimental features.

## Contents

### Core Design
- [Design Philosophy](DESIGN.md) - Architecture decisions and trade-offs
- [Multi-Paradigm Architecture](PARADIGMS.md) - OOP, IOP, COP
- [Ecosystem Design](ECOSYSTEM_DESIGN.md) - Standard library and PRM architecture

### Pillars
- [Pillars 1-5 Overview](01_core_pillars_1_to_5.md) - Intent, Context, ASR, Security, Chrono
- [Pillars 6-10 Overview](02_core_pillars_6_to_10.md) - Distributed, AI, Quantum, GPU, Zero-Trust

### Individual Pillars
1. [Intent-Oriented](01_intent_oriented.md) - IOP: `intent`/`resolver` — **Stable**
2. [Context-Aware](02_context_aware.md) - COP: `context`/`layer`/`activate` — **Stable**
3. [Self-Healing (ASR)](03_asr.md) - `resilient`/`recovery` — **Stable**
4. [Intrinsic Security](04_intrinsic_security.md) - Taint analysis — **Partial**
5. [Chrono-Native](05_chrono_native.md) - Temporal variables — **Frontend Only**
6. [Distributed](06_distributed_primitives.md) - Node types — **Stub**
7. [AI-Native](07_ai_native.md) - Tensors/Models — **Partial**
8. [Quantum-Ready](08_quantum_ready.md) - Qubit syntax — **Stub**
9. [Hardware Math](09_hardware_math.md) - GPU kernels — **Stub**
10. [Zero-Trust](10_zero_trust.md) - Identity verification — **Stub**

### Experimental
- [ASR Runtime](ASR_RUNTIME.md) - Self-healing runtime
- [ProXWire](ProXWire.md) - Distributed messaging

---

**Status**: Active design docs
