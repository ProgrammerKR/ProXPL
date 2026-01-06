# ProXPL VS Code Extension

Professional language support for **ProXPL** (ProX Programming Language) in Visual Studio Code. This extension provides a rich development experience with syntax highlighting, code snippets, and language configuration.

## ✨ Features

### 🎨 Syntax Highlighting
Comprehensive syntax highlighting for all ProXPL language features:
- **Keywords**: Control flow (`if`, `while`, `for`), declarations (`func`, `let`, `const`), and modules (`use`).
- **Types**: Built-in types (`int`, `float`, `string`, `bool`, `list`, `dict`).
- **Literals**: Numbers (hex, binary, float), strings (escapes), and booleans.
- **Comments**: Single-line (`//`) and multi-line (`/* ... */`).

### 🧩 Code Snippets
Boost your productivity with built-in snippets for common patterns:
- `main` - scaffolding for the main entry point
- `func` - function declaration structure
- `if`, `ifelse` - conditional logic
- `for`, `while` - loop structures
- `print` - quick print statement
- `try` - error handling block
- `use` - module import
- `doc` - Documentation comments

### 🛠️ Language Configuration
- **Auto-closing pairs**: Automatically closes braces `{}`, brackets `[]`, parentheses `()`, and quotes.
- **Bracket matching**: Visual indicators for matching brackets.
- **Comment toggling**: Use `Ctrl+/` (or `Cmd+/`) to toggle comments.
- **Folding**: Collapse and expand code blocks.

## 📦 Installation

### From Marketplace
(Coming Soon to VS Code Marketplace)

### Manual Installation (.vsix)
1. Download the latest `.vsix` release from the [ProXPL Releases](https://github.com/ProgrammerKR/ProXPL/releases).
2. Open VS Code.
3. Go to the Extensions view (`Ctrl+Shift+X`).
4. Click the `...` menu and select **"Install from VSIX..."**.
5. Select the downloaded file.

### For Developers (Source)
1. Clone the repository:
   ```bash
   git clone https://github.com/ProgrammerKR/ProXPL.git
   cd ProXPL/extension
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Open in VS Code:
   ```bash
   code .
   ```
4. Press `F5` to launch the Extension Development Host.

## 🚀 Usage

Create a file with `.prox` or `.pxpl` extension to activate the extension.

```javascript
// hello.prox
func main() {
    print("Hello, ProXPL!");
}
```

## ⌨️ Shortcuts

| Snippet Prefix | Description |
|----------------|-------------|
| `main` | Create main function |
| `func` | Define a new function |
| `print` | Print statement |
| `use` | Import a module |
| `let` | Variable declaration |

## 🤝 Contributing

Found a bug or want to improve the syntax highlighting? Contributions are welcome!

1. Fork the repo.
2. Create a feature branch.
3. Submit a Pull Request.

## 📄 License

This extension is part of the ProXPL project and is licensed under the [MIT License](../LICENSE).
