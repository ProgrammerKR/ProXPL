# ProXPL v0.5.0 Release Notes

## 🚀 Highlights

ProXPL v0.5.0 marks a significant milestone in our journey towards a robust and developer-friendly language ecosystem. This release introduces the foundation for **Language Server Protocol (LSP)** and **Debugger Adapter Protocol (DAP)** support, laying the groundwork for a world-class editing and debugging experience. We also unified our module system with the new `use` keyword and rebranded our extension to **ProX Studio Alpha**.

## ✨ New Features

### 🧠 Language Server & Debugging
- **LSP Foundation**: Initial implementation of the Language Server Protocol structure. This is the first step towards features like autocompletion, go-to-definition, and real-time error checking.
- **DAP Scaffolding**: Setup for the Debugger Adapter Protocol to enable debugging support in VS Code and other editors.

### 📦 Module System
- **`use` Keyword**: Introduced the `use` keyword for importing modules, replacing previous experimental syntaxes.
- **Generic Module Loading**: Enhanced the runtime to support loading modules dynamically.
- **`std.io`**: Improvements to the standard IO module.

### 🎨 Extension & Branding
- **ProX Studio Alpha**: The VS Code extension has been rebranded to reflect its growing capabilities.
- **Icon Theme**: Added a comprehensive icon theme supporting 100+ file types (JS, TS, Python, C++, etc.) via `vscode-material-icon-theme` integration.
- **`.pxcf` Support**: Added explicit support and icons for ProX Configuration Files.

### 🌐 Web & Documentation
- **SEO Improvements**: Added Google Site Verification and fixed sitemap issues for better discoverability.
- **Documentation**: Continued refinement of documentation for better clarity on new features.

## 🛠 Fixes & Improvements
- **VSIX Packaging**: Resolved issues preventing successful packaging of the VS Code extension.
- **Build System**: Fixed CMake configuration issues for smoother builds on Windows.
- **Versioning**: Unified version strings across the project to `0.5.0`.

## 📦 Installation

To install the latest version of the VS Code extension, download the `.vsix` file from the release assets and install it manually in VS Code, or search for "ProX Studio Alpha" in the marketplace once published.

## 🤝 Contributors

A huge thanks to the development team for their hard work on this release!
