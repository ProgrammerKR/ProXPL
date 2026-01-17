# Pillar 9: Hardware-Accelerated Math

## Concept
Hardware-Accelerated Math exposes high-performance computing capabilities directly. It allows developers to define blocks of code intended for GPU execution (kernels) and high-dimensional data types (tensors, matrices).

## Syntax

### GPU Block
```javascript
gpu kernel MatMul {
    // Code to run on GPU
}
```

### Math Types
- `tensor`: N-dimensional array.
- `matrix`: 2D array specialized for linear algebra.

## Usage
The compiler identifies `gpu` blocks and compiles them to SPIR-V, CUDA, or OpenCL depending on the backend, abstracting the low-level details of graphics API interoperability.
