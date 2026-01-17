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
