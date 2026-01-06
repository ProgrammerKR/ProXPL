# std.gc

The `std.gc` module provides control over the ProXPL garbage collector and access to memory statistics.

**Note**: This module is part of the standard library but interacts directly with the runtime's memory management system.

## Functions

### collect()

Force a garbage collection cycle immediately.

**Signature**
```javascript
collect() -> int
```

**Returns**
- `int`: The number of bytes freed during this collection cycle.

**Example**
```javascript
use std.gc;

// Force cleanup
let freed = gc.collect();
print("Freed bytes: " + to_string(freed));
```

### stats()

Retrieve current memory statistics from the garbage collector.

**Signature**
```javascript
stats() -> list
```

**Returns**
- `list`: A list containing `[bytesAllocated, nextGCThreshold]`.
    - `bytesAllocated` (index 0): Total bytes currently allocated by the VM.
    - `nextGCThreshold` (index 1): The allocation threshold that will trigger the next automatic GC.

**Example**
```javascript
use std.gc;

let stats = gc.stats();
print("Current Usage: " + to_string(stats[0]) + " bytes");
print("Next GC at: " + to_string(stats[1]) + " bytes");
```

### usage()

Quickly check current memory usage.

**Signature**
```javascript
usage() -> int
```

**Returns**
- `int`: Total bytes currently allocated.

**Example**
```javascript
use std.gc;
print("Memory: " + to_string(gc.usage()));
```
