# ProXPL Compiler Architecture

## Overview
The compiler uses a multi-pass architecture:

1. **Lexical Analysis (Lexer)**: Transforms source text into a stream of tokens.
2. **Parsing (Parser)**: Consumes tokens to build an Abstract Syntax Tree (AST).
3. **Semantic Analysis (TypeChecker)**: Validates scope, variable declarations, and (in future) types.
4. **Intermediate Representation (IR)**: Translates AST into a flattened SSA-inspired 3-address code format.
5. **Optimization**: Performs passes on IR (Constant Folding, Dead Code Elimination).
6. **Code Generation / Execution (VM)**: The VM executes the IR instructions directly (or bytecode in future).

## Components

- `core/lexer`: Tokenizer
- `core/parser`: Recursive descent parser
- `core/semantic`: Symbol table and semantic checks
- `core/ir`: IR definitions (Opcodes, Blocks, Modules)
- `core/optimizer`: IR-to-IR transformation passes
- `core/vm`: Stack/Register hybrid execution engine
