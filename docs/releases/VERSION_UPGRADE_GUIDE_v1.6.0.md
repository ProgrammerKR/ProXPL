# ProXPL v1.6.0 Upgrade Guide

Welcome to ProXPL v1.6.0! This release marks a significant milestone as we have fully integrated the core advanced paradigms into the ProXPL Virtual Machine and Bytecode generator.

This guide covers the necessary adjustments required when upgrading your codebase from v1.5.0.

## 1. Intent-Oriented Programming

In v1.5.0, intent declarations and resolvers were parsed but ignored by the compiler backend. In v1.6.0, they are fully functional and emit runtime opcodes.

### Syntax Requirement
Intent declarations now strictly require a trailing semicolon (`;`).

**v1.5.0 (Tolerated but ignored):**
```proxpl
intent ProcessData(data)
```

**v1.6.0 (Required):**
```proxpl
intent ProcessData(data);
```

### Resolver Matching
Ensure you use the `matches` keyword (not `resolves` or other variants) when binding a resolver to an intent.

```proxpl
resolver DataHandler matches ProcessData {
    print("Handling data!");
}
```

## 2. Context-Aware Polymorphism

Contexts and Layers are now strictly validated and execute proper environment scoping at runtime.

### Block Activation
The `activate` statement now requires a block to encapsulate the active context. It cannot be used as a standalone statement.

**v1.5.0 (Tolerated but ignored):**
```proxpl
activate AdminTheme;
```

**v1.6.0 (Required):**
```proxpl
activate AdminTheme {
    // Operations inside this block run with AdminTheme layers applied
    renderUI();
}
```

## 3. Autonomic Self-Healing (ASR)

The `resilient` keyword now provides genuine zero-cost exception unwinding using `setjmp`/`longjmp` under the hood in the VM.

Code placed inside a `resilient` block will no longer cause fatal crashes when operations like division-by-zero occur. Instead, they will recover silently and continue execution after the block. 

No syntax changes are required, but you should review your error-handling logic as runtime errors that previously crashed the process will now be caught and recovered if inside a `resilient` scope.
