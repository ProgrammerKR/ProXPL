# ProX Resource Manager (PRM) & ProXPL Usage Guide

## ✅ Prerequisites
Ensure you have installed **ProXPL** using the Windows Installer (`ProXPL_Installer_vX.X.X.exe`).
- This installs `proxpl.exe` (Compiler) and `prm.exe` (Package Manager).
- It automatically adds them to your **System PATH**.

---

## 💻 1. Using from Terminal (CMD / PowerShell / Bash)

You can run these commands from *VS Code Terminal*, *Command Prompt*, or *PowerShell*.

### ➤ Verify Installation
Check if everything is set up correctly:
```bash
prm --version
proxpl --version
```

### ➤ Quick Start (Project Mode)
Best for building applications.
```bash
# 1. Create a new project
prm init my_project

# 2. Go into directory
cd my_project

# 3. Run the project
prm run
```

### ➤ Running a Single File
Best for quick scripts or testing.
```bash
proxpl src/main.prox
```

### ➤ Helper Commands
| Command | Description |
| :--- | :--- |
| `prm help` | Show all available commands. |
| `prm doctor` | Check for issues in your setup. |
| `prm install <pkg>`| Install a package (e.g., `prm install std.net`). |
| `prm build` | Build the project for production. |
| `prm test` | Run project tests. |

---

## 🆚 2. Using with VS Code

The ProXPL extension provides seamless integration.

### ➤ Run Current File
1. Open any `.prox` file.
2. **Right-Click** anywhere in the editor.
3. Select **"Run ProXPL File"**.
4. *Shortcut:* Press `Ctrl + F5`.

This will open a **dedicated ProXPL terminal** and run your code instantly.

---

## ❓ Troubleshooting
**"Command not found" error?**
- Restart your terminal or VS Code after installation.
- Ensure you checked **"Add to PATH"** during installation.
