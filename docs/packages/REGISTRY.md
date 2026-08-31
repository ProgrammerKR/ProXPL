# ProXPL Registry & Module Hosting

This document outlines the current state and future plans for distributing and hosting ProXPL modules and standard libraries.

## Current State (v0.1)

Currently, the ProXPL Standard Library (`std.*`) is **embedded** directly into the `proxpl` interpreter. This means:
- You do not need to download `std` modules separately.
- They are available globally via the `use` statement (e.g., `use std.io;`).
- The source code for these native modules resides in `src/stdlib/`.

## Registry Plan (Future)

We plan to introduce a proper package registry (like npm or crates.io) for community packages.

### Design
- **Registry URL:** `https://registry.proxpl.org` (Placeholder)
- **Tooling:** The `prm` (ProXPL Resource Manager) CLI will handle publishing and installing packages.
- **Hosting:** Packages will be hosted as compressed archives (`.tar.gz` or `.zip`).

### Workflow (Draft)

1. **Publishing a Package:**
   ```bash
   prm login
   prm publish
   ```
2. **Installing a Package:**
   ```bash
   prm install user/package
   ```
   This will download the package to a local `prox_modules/` directory.

### Standard Library Updates
The `std` library will remain distributed with the interpreter to ensure a consistent core usage. However, entirely new standard modules might be distributed as separate packages in the future to keep the core lightweight.

## What you need to do now
For your current usage:
1. **Rebuild the Interpreter:** Since we just added the native support for `std`, you must rebuild the project using CMake.
2. **Use the Standard Library:** Simply `use std.io;` in your scripts. No external hosting is needed yet.
3. **Third-Party Libraries:** For now, you can manually clone repositories into a `libs/` folder and `use` them by path.
