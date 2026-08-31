# Language Reference

Quick syntax reference for ProXPL.

## Variables and Declarations

```javascript
let x = 42;                    // Mutable
const PI = 3.14;               // Immutable
let count: int = 0;            // With type annotation
```

## Control Flow

```javascript
if (condition) { } else { }
while (condition) { }
for (let i = 0; i < 10; i++) { }
switch (val) { case 1: ... default: ... }
break; continue;
```

## Functions

```javascript
func name(params) { body }
async func asyncName(params) { body }
```

## Classes and OOP

```javascript
class Name {
    func method() { }
}
class Child extends Parent { }
```

## Intent-Oriented

```javascript
intent Name(params);
resolver Name matches Intent { }
```

## Context-Oriented

```javascript
context Name {
    layer Name { func ... }
}
activate ContextName { ... }
```

## Modules

```javascript
use std.module;
import ... from ...;
```

## Error Handling

```javascript
try { } catch (e) { } finally { }
```

---

**Status**: Stable
**See Also**: [Specification](SPEC.md), [Built-in Functions](BUILTINS.md)
