# AI-Native Integration

Define, train, and run ML models natively.

## Syntax

```proxpl
let matrix = [[1, 2], [3, 4]];
let result = matrix @ [[0, 1], [1, 0]];  // Matrix multiplication
```

## Features

- `@` operator for matrix/tensor multiplication
- Tensors via nested bracket syntax
- Element-wise arithmetic

## Status

**PARTIALLY_IMPLEMENTED** - Tensors (`OP_MAKE_TENSOR`) are actually implemented! VM has `performTensorArithmetic` supporting element-wise ops and matrix multiplication. Neural Network model training (`model`, `train`) remains a stub.

---

**Status**: Partial
