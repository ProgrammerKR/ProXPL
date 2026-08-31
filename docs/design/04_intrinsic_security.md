# Intrinsic Security

Taint analysis and `sanitize()` primitives baked into the type system.

## Overview

ProXPL provides intrinsic security through type-level taint tracking.

## Syntax

```proxpl
let tainted_data: tainted string = userInput;
let clean_data = sanitize(tainted_data);
```

## Features

- `tainted` type qualifier
- `sanitize()` function
- `pure` function declarations
- Information flow control at type-check level

## Status

**PARTIALLY_IMPLEMENTED** - Type Checker tracks `isTainted` flags and attempts basic IFC. This is largely static. The VM has no dynamic taint tracking.

---

**Status**: Partial
