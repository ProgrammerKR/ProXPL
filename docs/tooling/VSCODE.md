# VS Code Extension

Official ProXPL VS Code extension with 1100+ file icons and full language support.

## Features

- **1100+ File Icons**: Material Icon Theme integration
- **Syntax Highlighting**: Full ProXPL syntax support
- **Language Server**: LSP integration for code completion and diagnostics
- **Code Formatting**: `prox fmt` integration
- **Debug Support**: VM debug protocol integration

## Installation

Install from the VS Code marketplace:
1. Open Extensions (`Ctrl+Shift+X`)
2. Search "ProXPL"
3. Click Install

## Features

### Language Server Protocol

The LSP provides:
- Go-to-definition
- Auto-completion
- Real-time diagnostics
- Hover information
- Symbol navigation

### Formatter

Integrated `prox fmt` support for consistent code style.

### Diagnostics

Enhanced error codes (`E0412`, `E0308`), caret indicators, and fuzzy typo suggestions.

## Configuration

```json
{
    "proxpl.enable": true,
    "proxpl.formatOnSave": true,
    "proxpl.lsp.enabled": true
}
```

---

**Status**: Stable
