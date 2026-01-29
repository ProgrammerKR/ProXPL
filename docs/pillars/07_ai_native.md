# Pillar 7: AI-Native Integration

## Concept
AI-Native Integration makes Machine Learning a first-class citizen. Instead of importing external libraries like TensorFlow or PyTorch, ProXPL allows defining models, training loops, and predictions directly in the syntax.

## Syntax

### Model Declaration
```javascript
model SentimentAnalysis {
    // Model architecture and hyperparameters
}
```

### Operations
```javascript
// Train the model
train SentimentAnalysis with dataset;

// Make predictions
let score = predict("This is great") using SentimentAnalysis;
```

## Usage
This abstracts the complexity of ML pipelines, allowing the compiler to optimize mathematical operations (tensor math) and hardware usage (GPU offloading) automatically.

## Tensor Support
ProXPL treats tensors as first-class citizens, enabling seamless mathematical operations without external dependencies.

### Tensor Literals
Define multi-dimensional arrays using nested bracket syntax:
```javascript
let vector = [1, 2, 3];              // 1D tensor (shape: 3)
let matrix = [[1, 2], [3, 4]];       // 2D tensor (shape: 2x2)
let tensor3d = [[[1, 2], [3, 4]], [[5, 6], [7, 8]]]; // 3D tensor
```

### Matrix Multiplication
Use the `@` operator for matrix/tensor multiplication:
```javascript
let A = [[1, 2], [3, 4]];
let B = [[5, 6], [7, 8]];
let C = A @ B;  // Matrix multiplication

let v1 = [1, 2, 3];
let v2 = [4, 5, 6];
let dot = v1 @ v2;  // Dot product: 32
```

### Use Cases
- Neural network layer operations
- Linear algebra computations
- Image processing pipelines
- Scientific computing
