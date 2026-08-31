# Standard Library Implementation

Detailed documentation of native standard library implementations.

## Native Function Interface

```c
typedef Value (*NativeFn)(int argCount, Value *args);
typedef struct {
    ObjString *name;
    NativeFn function;
} NativeFunction;
```

## Module Structure

```
stdlib/
├── io_native.c          # I/O functions
├── math_native.c        # Math functions
├── string_native.c      # String functions
├── convert_native.c     # Type conversion
└── system_native.c      # System functions
```

## Registration

```c
void registerAll(VM *vm) {
    registerIOFunctions(vm);
    registerMathFunctions(vm);
    registerStringFunctions(vm);
}
```

---

**Status**: Stable
**See Also**: [Reference](../standard-library/REFERENCE.md), [GC](GC.md)
