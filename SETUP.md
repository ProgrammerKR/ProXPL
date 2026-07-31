# Setting up ProXPL for Development

This guide covers everything you need to know to set up your environment and build the ProXPL compiler from source.

## Prerequisites

To build ProXPL, your system must have the following dependencies installed:

1. **C/C++ Compiler**: 
   - **Windows**: MSVC (Visual Studio 2022) with C++ Desktop Development workload.
   - **Linux**: GCC (v9+) or Clang (v10+).
2. **CMake**: v3.15 or higher.
3. **LLVM (v18+)**: Required for the backend and optimization passes.
4. **Git**: To clone the repository and its submodules (if any).

*Note: Dependencies like `mimalloc` are fetched automatically by CMake during the build process.*

---

## 💻 Windows Setup Guide

### 1. Install Visual Studio & Build Tools
Download and install [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/).
During installation, select the **"Desktop development with C++"** workload. Under the "Installation details" panel on the right, make sure at least the following optional components are selected:
- **MSVC Build Tools for x64/x86 (Latest)**
- **C++ CMake tools for Windows**
- **Windows 11 SDK** (or Windows 10 SDK, e.g., 10.0.26100.0)

*(Other components shown in the installer like IntelliCode, C++ profiling tools, AddressSanitizer, and vcpkg are great to have for development but are strictly optional just for building).*

### 2. Install LLVM 18
You need the LLVM binaries and development headers. The easiest way is using a package manager like `winget` or downloading the installer.
- **Using Winget**:
  ```powershell
  winget install LLVM.LLVM
  ```
- **Manual Download**: 
  Go to the [LLVM GitHub Releases](https://github.com/llvm/llvm-project/releases) page and download `LLVM-18.x.x-win64.exe`. Make sure to check "Add LLVM to the system PATH" during installation.

### 3. Build ProXPL
Open **"x64 Native Tools Command Prompt for VS 2022"** (Search for it in the Start Menu) and run:

```cmd
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

:: Configure the project with CMake
cmake -S . -B build

:: Build the project in Release mode
cmake --build build --config Release
```

Your compiled executables (`proxpl.exe`, etc.) will be available inside the `build/Release/` directory.

---

## 🐧 Linux Setup Guide (Ubuntu/Debian)

### 1. Install Build Tools & CMake
Open your terminal and install the essential build tools:
```bash
sudo apt update
sudo apt install build-essential cmake git
```

### 2. Install LLVM 18
Ubuntu's default repositories might not have LLVM 18. You can use the official LLVM bash script to install it:

```bash
# Download and execute the LLVM automatic installation script
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18

# Install additional LLVM development headers required for building
sudo apt install llvm-18-dev libffi-dev
```

### 3. Build ProXPL
Clone the repository and build using CMake:

```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

# Configure the project
cmake -S . -B build

# Build the project
cmake --build build --config Release
```

Your compiled executables will be available inside the `build/` directory.

---

## 🛠️ Troubleshooting

- **CMake can't find LLVM**: 
  If CMake complains about `LLVM_DIR` not being found, you can manually point it to your LLVM installation during the configuration step:
  ```bash
  cmake -S . -B build -DLLVM_DIR="C:/Program Files/LLVM/lib/cmake/llvm"
  ```
  *(Adjust the path based on where LLVM was installed on your system).*

- **Linker errors on Windows**: 
  Make sure you are using the **x64 Native Tools Command Prompt** and not the standard Command Prompt/PowerShell, as MSVC requires specific environment variables to link properly.

- **Missing `mimalloc` errors**: 
  CMake fetches `mimalloc` automatically via `FetchContent`. If this fails, ensure you have an active internet connection during the `cmake -S . -B build` step.
