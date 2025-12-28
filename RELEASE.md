# ProXPL Release v0.2.0

**Date:** December 28, 2024
**Version:** 0.2.0
**Status:** Stable / Production-Ready Core

## 🚀 Highlights

This release focuses on **Critical Stability** and **Build System Reliability**. We have resolved major crashes in the Virtual Machine and Garbage Collector, ensuring that complex algorithms (like recursion and extensive object allocation) run reliably.

## ✨ New Features & Improvements

- **VS Code Extension Update**: The ProXPL extension is now version `0.2.0` and compatible with modern VS Code versions (`^1.107.0`).
- **Robust Benchmarking**: The benchmark suite is now more stable, with flaky tests removed and the runner hardened against memory errors.
- **Improved Build System**: `CMakeLists.txt` and build scripts have been polished for better cross-platform support (Windows/macOS/Linux).

## 🐛 Bug Fixes

- **CRITICAL: GC Use-After-Free**: Fixed a severe bug where the Garbage Collector failed to mark active call frames, causing the VM to crash when collecting memory during function execution.
- **CRITICAL: VM Stack Overflow**: Replaced hardcoded stack limit (64 frames) with `FRAMES_MAX` (1024), allowing for deep recursion.
- **CRITICAL: VM Dispatch Table**: Fixed a misalignment in the opcode dispatch table (missing `OP_NOP`), which caused `SIGSEGV` and completely broken execution flow.
- **macOS Build**: Fixed missing `<stdlib.h>` include causing compilation errors.

## 📦 Artifacts

- **Windows**: `proxpl.exe`, `proxpl-0.2.0.vsix`
- **Linux/macOS**: `proxpl` binary

## 🔧 Upgrading

To use this version, pull the latest changes from the `main` branch or download the pre-built binaries from the GitHub Actions status page.
