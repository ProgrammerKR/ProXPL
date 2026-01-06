# ProXPL VS Code Extension Development Guide

**Professional Syntax Highlighting and Developer Experience**

This guide provides complete instructions for creating a professional VS Code extension for the ProXPL programming language, including syntax highlighting, snippets, and IntelliSense support.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Initial Setup](#initial-setup)
3. [Project Structure](#project-structure)
4. [Configuration](#configuration)
5. [Syntax Highlighting](#syntax-highlighting)
6. [Code Snippets](#code-snippets)
7. [Language Configuration](#language-configuration)
8. [Testing](#testing)
9. [Packaging & Publishing](#packaging--publishing)
10. [Advanced Features](#advanced-features)

---

## Prerequisites

Before you begin, ensure you have the following installed:

- **Node.js** (v14 or higher): [Download](https://nodejs.org/)
- **npm** (comes with Node.js)
- **Visual Studio Code**: [Download](https://code.visualstudio.com/)
- **Yeoman** and **VS Code Extension Generator**:

```bash
npm install -g yo generator-code
```

- **vsce** (VS Code Extension Manager):

```bash
npm install -g vsce
```

---

## Initial Setup

### Step 1: Generate Extension Scaffold

Use the Yeoman generator to create the extension structure:

```bash
# Navigate to your extensions directory
cd ~/vscode-extensions  # or your preferred location

# Run the generator
yo code
```

### Step 2: Generator Prompts

Answer the prompts as follows:

```
? What type of extension do you want to create?
  → New Language Support

? What's the name of your language?
  → ProXPL

? What's the identifier of your language?
  → proxpl

? What's the name of your extension?
  → proxpl-language-support

? What's the description of your extension?
  → Syntax highlighting and language support for ProXPL

? What's your publisher name?
  → your-publisher-name

? Initialize a git repository?
  → Yes
```

This creates a directory structure:

```
proxpl-language-support/
├── .vscode/
│   └── launch.json
├── language-configuration.json
├── syntaxes/
│   └── proxpl.tmLanguage.json
├── package.json
├── README.md
└── CHANGELOG.md
```

---

## Project Structure

### Directory Layout

```
proxpl-language-support/
├── .vscode/
│   └── launch.json              # Debug configuration
├── syntaxes/
│   └── proxpl.tmLanguage.json   # TextMate grammar
├── snippets/
│   └── proxpl.json              # Code snippets
├── language-configuration.json   # Language features
├── package.json                  # Extension manifest
├── README.md                     # Extension documentation
├── CHANGELOG.md                  # Version history
└── icon.png                      # Extension icon (128x128)
```

---

## Configuration

### package.json

Update `package.json` with ProXPL-specific configuration:

```json
{
  "name": "proxpl-language-support",
  "displayName": "ProXPL Language Support",
  "description": "Syntax highlighting and language support for ProXPL",
  "version": "1.0.0",
  "publisher": "your-publisher-name",
  "icon": "icon.png",
  "repository": {
    "type": "git",
    "url": "https://github.com/ProgrammerKR/ProXPL"
  },
  "engines": {
    "vscode": "^1.75.0"
  },
  "categories": [
    "Programming Languages"
  ],
  "keywords": [
    "proxpl",
    "prox",
    "syntax",
    "highlighting"
  ],
  "contributes": {
    "languages": [
      {
        "id": "proxpl",
        "aliases": [
          "ProXPL",
          "prox"
        ],
        "extensions": [
          ".prox"
        ],
        "configuration": "./language-configuration.json",
        "icon": {
          "light": "./icon.png",
          "dark": "./icon.png"
        }
      }
    ],
    "grammars": [
      {
        "language": "proxpl",
        "scopeName": "source.proxpl",
        "path": "./syntaxes/proxpl.tmLanguage.json"
      }
    ],
    "snippets": [
      {
        "language": "proxpl",
        "path": "./snippets/proxpl.json"
      }
    ]
  }
}
```

**Key Sections:**

- **`contributes.languages`**: Defines the language identifier, file extensions, and configuration
- **`contributes.grammars`**: Links the TextMate grammar file for syntax highlighting
- **`contributes.snippets`**: Provides code snippets for common patterns

---

## Syntax Highlighting

### TextMate Grammar

Create `syntaxes/proxpl.tmLanguage.json` with comprehensive syntax highlighting:

```json
{
  "$schema": "https://raw.githubusercontent.com/martinring/tmlanguage/master/tmlanguage.json",
  "name": "ProXPL",
  "scopeName": "source.proxpl",
  "patterns": [
    {
      "include": "#comments"
    },
    {
      "include": "#keywords"
    },
    {
      "include": "#types"
    },
    {
      "include": "#strings"
    },
    {
      "include": "#numbers"
    },
    {
      "include": "#booleans"
    },
    {
      "include": "#operators"
    },
    {
      "include": "#functions"
    },
    {
      "include": "#constants"
    }
  ],
  "repository": {
    "comments": {
      "patterns": [
        {
          "name": "comment.line.double-slash.proxpl",
          "match": "//.*$"
        },
        {
          "name": "comment.block.proxpl",
          "begin": "/\\*",
          "end": "\\*/"
        }
      ]
    },
    "keywords": {
      "patterns": [
        {
          "name": "keyword.control.proxpl",
          "match": "\\b(if|else|while|for|switch|case|default|break|continue|return|try|catch|finally|throw|defer)\\b"
        },
        {
          "name": "keyword.declaration.proxpl",
          "match": "\\b(let|const|func|class|enum|struct|interface|abstract)\\b"
        },
        {
          "name": "keyword.module.proxpl",
          "match": "\\b(import|from|export|as|use)\\b"
        },
        {
          "name": "keyword.other.proxpl",
          "match": "\\b(this|super|static|public|private|protected|extends|implements|in|is|typeof|native|async|await)\\b"
        }
      ]
    },
    "types": {
      "patterns": [
        {
          "name": "storage.type.proxpl",
          "match": "\\b(int|float|string|bool|void|null|list|dict|set|bytes|error)\\b"
        }
      ]
    },
    "strings": {
      "patterns": [
        {
          "name": "string.quoted.double.proxpl",
          "begin": "\"",
          "end": "\"",
          "patterns": [
            {
              "name": "constant.character.escape.proxpl",
              "match": "\\\\(n|t|r|\\\\|\"|'|x[0-9A-Fa-f]{2}|u[0-9A-Fa-f]{4})"
            }
          ]
        },
        {
          "name": "string.quoted.single.proxpl",
          "begin": "'",
          "end": "'",
          "patterns": [
            {
              "name": "constant.character.escape.proxpl",
              "match": "\\\\(n|t|r|\\\\|\"|'|x[0-9A-Fa-f]{2}|u[0-9A-Fa-f]{4})"
            }
          ]
        }
      ]
    },
    "numbers": {
      "patterns": [
        {
          "name": "constant.numeric.hex.proxpl",
          "match": "\\b0[xX][0-9A-Fa-f]+\\b"
        },
        {
          "name": "constant.numeric.binary.proxpl",
          "match": "\\b0[bB][01]+\\b"
        },
        {
          "name": "constant.numeric.octal.proxpl",
          "match": "\\b0[oO][0-7]+\\b"
        },
        {
          "name": "constant.numeric.float.proxpl",
          "match": "\\b[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?\\b"
        },
        {
          "name": "constant.numeric.integer.proxpl",
          "match": "\\b[0-9]+\\b"
        }
      ]
    },
    "booleans": {
      "patterns": [
        {
          "name": "constant.language.boolean.proxpl",
          "match": "\\b(true|false)\\b"
        },
        {
          "name": "constant.language.null.proxpl",
          "match": "\\bnull\\b"
        }
      ]
    },
    "operators": {
      "patterns": [
        {
          "name": "keyword.operator.arithmetic.proxpl",
          "match": "(\\+|\\-|\\*|\\/|%|\\*\\*)"
        },
        {
          "name": "keyword.operator.comparison.proxpl",
          "match": "(==|!=|<|>|<=|>=)"
        },
        {
          "name": "keyword.operator.logical.proxpl",
          "match": "(&&|\\|\\||!)"
        },
        {
          "name": "keyword.operator.bitwise.proxpl",
          "match": "(&|\\||\\^|~|<<|>>)"
        },
        {
          "name": "keyword.operator.assignment.proxpl",
          "match": "(=|\\+=|\\-=|\\*=|\\/=|%=|\\*\\*=|&=|\\|=|\\^=|<<=|>>=)"
        },
        {
          "name": "keyword.operator.other.proxpl",
          "match": "(\\?|:|\\?\\.|\\.\\.|\\+\\+|\\-\\-|\\?\\?)"
        }
      ]
    },
    "functions": {
      "patterns": [
        {
          "name": "entity.name.function.proxpl",
          "match": "\\b([a-zA-Z_][a-zA-Z0-9_]*)\\s*(?=\\()"
        },
        {
          "name": "support.function.builtin.proxpl",
          "match": "\\b(print|input|read_file|write_file|append_file|abs|ceil|floor|round|max|min|pow|sqrt|log|sin|cos|tan|random|randint|len|upper|lower|trim|split|join|replace|push|pop|insert|remove|range|sort|reverse|keys|values|exit|env|platform|version|to_int|to_float|to_string|to_bool|type|assert|hash)\\b"
        }
      ]
    },
    "constants": {
      "patterns": [
        {
          "name": "constant.other.proxpl",
          "match": "\\b[A-Z_][A-Z0-9_]*\\b"
        }
      ]
    }
  }
}
```

**Grammar Breakdown:**

- **`comments`**: Single-line (`//`) and multi-line (`/* */`) comments
- **`keywords`**: Control flow, declarations, modules, and other keywords
- **`types`**: Built-in types (int, float, string, bool, etc.)
- **`strings`**: Double and single-quoted strings with escape sequences
- **`numbers`**: Hex, binary, octal, float, and integer literals
- **`booleans`**: true, false, null
- **`operators`**: Arithmetic, comparison, logical, bitwise, assignment
- **`functions`**: Function calls and built-in functions
- **`constants`**: ALL_CAPS constants

---

## Code Snippets

Create `snippets/proxpl.json` with common code patterns:

```json
{
  "Function Definition": {
    "prefix": "func",
    "body": [
      "func ${1:functionName}(${2:params}) {",
      "\t${3:// function body}",
      "\treturn ${4:value};",
      "}"
    ],
    "description": "Create a function"
  },
  "If Statement": {
    "prefix": "if",
    "body": [
      "if (${1:condition}) {",
      "\t${2:// code}",
      "}"
    ],
    "description": "If statement"
  },
  "If-Else Statement": {
    "prefix": "ifelse",
    "body": [
      "if (${1:condition}) {",
      "\t${2:// if code}",
      "} else {",
      "\t${3:// else code}",
      "}"
    ],
    "description": "If-else statement"
  },
  "While Loop": {
    "prefix": "while",
    "body": [
      "while (${1:condition}) {",
      "\t${2:// code}",
      "}"
    ],
    "description": "While loop"
  },
  "For Loop": {
    "prefix": "for",
    "body": [
      "for (let ${1:i} = ${2:0}; ${1:i} < ${3:10}; ${1:i} = ${1:i} + 1) {",
      "\t${4:// code}",
      "}"
    ],
    "description": "For loop"
  },
  "Variable Declaration": {
    "prefix": "let",
    "body": "let ${1:varName} = ${2:value};",
    "description": "Declare a variable"
  },
  "Constant Declaration": {
    "prefix": "const",
    "body": "const ${1:CONST_NAME} = ${2:value};",
    "description": "Declare a constant"
  },
  "Print Statement": {
    "prefix": "print",
    "body": "print(${1:message});",
    "description": "Print to console"
  },
  "Try-Catch": {
    "prefix": "try",
    "body": [
      "try {",
      "\t${1:// code}",
      "} catch (${2:error}) {",
      "\t${3:// error handling}",
      "}"
    ],
    "description": "Try-catch block"
  },
  "Main Function": {
    "prefix": "main",
    "body": [
      "func main() {",
      "\t${1:// main code}",
      "}",
      "",
      "main();"
    ],
    "description": "Main function entry point"
  },
  "Import Module": {
    "prefix": "use",
    "body": "use ${1:module};",
    "description": "Import a module"
  },
  "List Declaration": {
    "prefix": "list",
    "body": "let ${1:listName} = [${2:items}];",
    "description": "Declare a list"
  },
  "Dictionary Declaration": {
    "prefix": "dict",
    "body": "let ${1:dictName} = {${2:\"key\": \"value\"}};",
    "description": "Declare a dictionary"
  }
}
```

**Snippet Features:**

- **Tab stops**: `${1:placeholder}` allows tabbing through fields
- **Placeholders**: Provide default values and hints
- **Multi-line**: Support for complex code structures

---

## Language Configuration

Create `language-configuration.json` for editor features:

```json
{
  "comments": {
    "lineComment": "//",
    "blockComment": ["/*", "*/"]
  },
  "brackets": [
    ["{", "}"],
    ["[", "]"],
    ["(", ")"]
  ],
  "autoClosingPairs": [
    { "open": "{", "close": "}" },
    { "open": "[", "close": "]" },
    { "open": "(", "close": ")" },
    { "open": "\"", "close": "\"", "notIn": ["string"] },
    { "open": "'", "close": "'", "notIn": ["string", "comment"] }
  ],
  "surroundingPairs": [
    ["{", "}"],
    ["[", "]"],
    ["(", ")"],
    ["\"", "\""],
    ["'", "'"]
  ],
  "folding": {
    "markers": {
      "start": "^\\s*//\\s*#?region\\b",
      "end": "^\\s*//\\s*#?endregion\\b"
    }
  },
  "wordPattern": "(-?\\d*\\.\\d\\w*)|([^\\`\\~\\!\\@\\#\\%\\^\\&\\*\\(\\)\\-\\=\\+\\[\\{\\]\\}\\\\\\|\\;\\:\\'\\\"\\,\\.\\<\\>\\/\\?\\s]+)",
  "indentationRules": {
    "increaseIndentPattern": "^((?!\\/\\/).)*(\\{[^}\"'`]*|\\([^)\"'`]*|\\[[^\\]\"'`]*)$",
    "decreaseIndentPattern": "^((?!.*?\\/\\*).*\\*/)?\\s*[\\)\\}\\]].*$"
  }
}
```

**Features Enabled:**

- **Comment toggling**: Ctrl+/ for line comments
- **Bracket matching**: Automatic bracket pairing
- **Auto-closing**: Automatic closing of brackets and quotes
- **Code folding**: Region-based code folding
- **Smart indentation**: Automatic indentation

---

## Testing

### Local Testing

1. **Open Extension in VS Code**:
   ```bash
   cd proxpl-language-support
   code .
   ```

2. **Launch Extension Development Host**:
   - Press `F5` or click "Run > Start Debugging"
   - A new VS Code window opens with the extension loaded

3. **Test the Extension**:
   - Create a new file: `test.prox`
   - Write ProXPL code and verify:
     - ✅ Syntax highlighting works
     - ✅ Comments are colored correctly
     - ✅ Keywords are highlighted
     - ✅ Snippets autocomplete
     - ✅ Brackets auto-close

4. **Debug Issues**:
   - Check the Debug Console for errors
   - Modify grammar files and reload (`Ctrl+R` in Extension Host)

### Test File Example

Create `test.prox` with comprehensive syntax:

```javascript
// ProXPL Test File
/* Multi-line comment
   Testing all features */

use std.math;

const PI = 3.14159;
let count = 0;

func fibonacci(n: int): int {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

func main() {
    let name = input("Enter name: ");
    print("Hello, " + name + "!");
    
    // Numbers
    let hex = 0xFF;
    let binary = 0b1010;
    let float = 3.14e-2;
    
    // Collections
    let numbers = [1, 2, 3, 4, 5];
    let person = {"name": "Alice", "age": 30};
    
    // Control flow
    for (let i = 0; i < 10; i = i + 1) {
        if (i % 2 == 0) {
            print("Even: " + to_string(i));
        } else {
            print("Odd: " + to_string(i));
        }
    }
    
    // Try-catch
    try {
        let result = fibonacci(10);
        print("Result: " + to_string(result));
    } catch (error) {
        print("Error: " + error);
    }
}

main();
```

---

## Packaging & Publishing

### Step 1: Prepare for Publishing

1. **Update README.md**:
   ```markdown
   # ProXPL Language Support
   
   Syntax highlighting and language support for ProXPL.
   
   ## Features
   - Syntax highlighting
   - Code snippets
   - Auto-closing brackets
   - Comment toggling
   
   ## Installation
   Install from VS Code Marketplace or download .vsix
   ```

2. **Update CHANGELOG.md**:
   ```markdown
   # Change Log
   
   ## [1.0.0] - 2024-12-23
   - Initial release
   - Syntax highlighting
   - Code snippets
   - Language configuration
   ```

3. **Add Icon** (optional):
   - Create `icon.png` (128x128 pixels)
   - Update `package.json` icon path

### Step 2: Package Extension

```bash
# Install vsce if not already installed
npm install -g vsce

# Package the extension
vsce package

# Output: proxpl-language-support-1.0.0.vsix
```

### Step 3: Test .vsix File

```bash
# Install locally
code --install-extension proxpl-language-support-1.0.0.vsix

# Verify installation
code --list-extensions | grep proxpl
```

### Step 4: Publish to Marketplace

1. **Create Publisher Account**:
   - Visit [Visual Studio Marketplace](https://marketplace.visualstudio.com/manage)
   - Create a publisher ID

2. **Get Personal Access Token**:
   - Go to [Azure DevOps](https://dev.azure.com/)
   - Create a PAT with "Marketplace (Manage)" scope

3. **Login with vsce**:
   ```bash
   vsce login your-publisher-name
   # Enter your PAT when prompted
   ```

4. **Publish**:
   ```bash
   vsce publish
   # Or publish with version bump
   vsce publish patch  # 1.0.0 -> 1.0.1
   vsce publish minor  # 1.0.0 -> 1.1.0
   vsce publish major  # 1.0.0 -> 2.0.0
   ```

5. **Verify**:
   - Visit marketplace: `https://marketplace.visualstudio.com/items?itemName=your-publisher-name.proxpl-language-support`

---

## Advanced Features

### 1. Language Server Protocol (LSP)

For advanced features like autocomplete, go-to-definition, and diagnostics, implement an LSP server:

**Structure:**
```
proxpl-language-support/
├── client/           # VS Code extension
└── server/           # LSP server
    └── src/
        └── server.ts
```

**Resources:**
- [LSP Guide](https://code.visualstudio.com/api/language-extensions/language-server-extension-guide)
- [LSP Specification](https://microsoft.github.io/language-server-protocol/)

### 2. Semantic Highlighting

Provide more accurate highlighting based on semantic analysis:

```json
{
  "contributes": {
    "semanticTokenTypes": [
      { "id": "variable", "description": "Variables" },
      { "id": "function", "description": "Functions" }
    ]
  }
}
```

### 3. Debugger Support

Add debugging capabilities:

```json
{
  "contributes": {
    "debuggers": [
      {
        "type": "proxpl",
        "label": "ProXPL Debug",
        "program": "./out/debugAdapter.js"
      }
    ]
  }
}
```

### 4. Task Provider

Integrate build tasks:

```json
{
  "contributes": {
    "taskDefinitions": [
      {
        "type": "proxpl",
        "required": ["task"],
        "properties": {
          "task": {
            "type": "string",
            "description": "The ProXPL task to execute"
          }
        }
      }
    ]
  }
}
```

---

## Troubleshooting

### Common Issues

**1. Syntax Highlighting Not Working**

- Verify `scopeName` in `tmLanguage.json` matches `package.json`
- Check regex patterns for syntax errors
- Reload Extension Host (`Ctrl+R`)

**2. Snippets Not Appearing**

- Ensure `snippets` path in `package.json` is correct
- Check JSON syntax in snippets file
- Verify language ID matches

**3. Extension Not Loading**

- Check `engines.vscode` version compatibility
- Review activation events in `package.json`
- Check Extension Host console for errors

**4. Publishing Fails**

- Verify publisher name is created
- Check PAT has correct permissions
- Ensure all required fields in `package.json` are filled

---

## Best Practices

1. **Version Control**: Use Git and semantic versioning
2. **Testing**: Test on multiple VS Code versions
3. **Documentation**: Keep README.md updated
4. **Performance**: Optimize regex patterns for speed
5. **Accessibility**: Use semantic token types
6. **Updates**: Regularly update dependencies

---

## Resources

- [VS Code Extension API](https://code.visualstudio.com/api)
- [TextMate Grammar](https://macromates.com/manual/en/language_grammars)
- [Yo Code Generator](https://github.com/Microsoft/vscode-generator-code)
- [VSCE Documentation](https://github.com/microsoft/vscode-vsce)
- [Extension Samples](https://github.com/microsoft/vscode-extension-samples)

---

## Next Steps

1. ✅ Create basic syntax highlighting
2. ✅ Add code snippets
3. ✅ Test locally
4. ✅ Package extension
5. 🔄 Implement LSP server (advanced)
6. 🔄 Add debugger support (advanced)
7. 🔄 Publish to marketplace

---

<p align="center">
  <b>ProXPL VS Code Extension - Professional Developer Experience</b>
</p>
