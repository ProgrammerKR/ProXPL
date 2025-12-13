# LSP Server (Skeleton)

This directory will contain the ProXPL LSP server implementation and protocol handling. Goals for the LSP server:
- Diagnostics, hover, go-to-definition
- Completion and semantic tokens
- Formatting and code actions

Plan
----
1. Define a robust AST and symbol table interface for LSP to query.
2. Implement a JSON-RPC based LSP server in a new `tools/lsp` project (Node.js/TypeScript or C).
3. Add tests and integrate into CI.
# ProXPL LSP - Starter Notes

This folder will contain the language server implementation (LSP) supporting:
- syntax highlighting
- auto-completion
- diagnostics
- go-to-definition
- signature help

Planned approach
- Use Node.js + TypeScript with `vscode-languageserver` for rapid development.
- Alternatively provide a lightweight Python `pygls` server for contributors preferring Python.

Next steps
1. Add grammar-based parser bindings (re-use `src/parser` AST).  
2. Implement basic diagnostics by running the existing parser and type checker.
3. Implement completion/signature by walking the AST and symbol tables.
