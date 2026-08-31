# Intent-Oriented Programming

Define *what* you want, not *how* to do it.

## Overview

Intent-Oriented Programming (IOP) shifts focus from implementation to goals. The runtime resolves intents to the best available implementation.

## Syntax

```proxpl
intent Storage(data: string, path: string);

resolver LocalStorage matches Storage {
    // Local disk implementation
}

resolver CloudStorage matches Storage {
    // Cloud sync implementation
}

// Runtime selects best implementation
Storage("database_backup", "/etc/config");
```

## Components

- **Intent**: Abstract declaration of a goal or service requirement
- **Resolver**: Concrete implementation that satisfies an intent
- **Decoupling**: Callers request an intent, runtime resolves to best implementation

## Status

**IMPLEMENTED** - Parser generates `STMT_INTENT_DECL` and `STMT_RESOLVER_DECL`. Type Checker tracks them. Bytecode Generator emits `OP_INTENT` and `OP_RESOLVER` opcodes, resolved in the VM dispatch loop.

---

**Status**: Stable
