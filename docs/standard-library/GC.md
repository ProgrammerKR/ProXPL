# Garbage Collector

Mark-and-sweep garbage collector for automatic memory management.

## Algorithm

```
1. MARK PHASE:
   - Start from roots (globals, stack, current frame)
   - Follow all reachable objects
   - Mark reachable objects

2. SWEEP PHASE:
   - Iterate through all objects
   - Free unmarked objects
   - Unmark marked objects for next cycle

3. THRESHOLD:
   - GC triggered when allocated memory exceeds threshold
   - Threshold doubles after each collection
```

## Key Functions

```c
void *reallocate(void *pointer, size_t oldSize, size_t newSize);
#define ALLOCATE(type, count) (type*)reallocate(NULL, 0, sizeof(type) * (count))
#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)
```

## String Interning

All strings are interned for memory efficiency:
- O(1) string comparison (pointer equality)
- Memory sharing for duplicate strings
- Fast hashing for dictionary keys

---

**Status**: Stable
**See Also**: [VM Architecture](../runtime/VM_ARCHITECTURE.md), [Reference](../standard-library/REFERENCE.md)
