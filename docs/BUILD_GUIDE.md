# ProXPL Build Guide

## Quick Start

### For the Impatient

**Unix/Linux/macOS**:
```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
make
./bin/prox examples/hello.prox
```

**Windows**:
```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
.\Release\prox.exe ..\examples\hello.prox
```

---

## Table of Contents

1. [Requirements](#requirements)
2. [Detailed Build Instructions](#detailed-build-instructions)
3. [Platform-Specific Guides](#platform-specific-guides)
4. [Build Configuration](#build-configuration)
5. [Running Tests](#running-tests)
6. [Troubleshooting](#troubleshooting)
7. [Advanced Options](#advanced-options)

---

## Requirements

### Minimum Requirements

- **C Compiler**: C99 compatible
  - GCC 7.0+ (Linux)
  - Clang 5.0+ (macOS, Linux)
  - MSVC 2015+ (Windows)
- **CMake**: 3.10 or later
- **Git**: For cloning repository

### Recommended

- **Build Tools**: GNU Make (Unix) or Ninja (cross-platform)
- **Memory**: 512MB free RAM
- **Disk Space**: 100MB for source and build

### Optional

- **Testing**: CUnit framework (for running tests)
- **Documentation**: Doxygen (to generate API docs)
- **Coverage**: gcov, lcov (for coverage reports)

---

## Detailed Build Instructions

### Step 1: Clone Repository

```bash
# Clone with HTTPS
git clone https://github.com/ProgrammerKR/ProXPL.git

# OR clone with SSH
git clone git@github.com:ProgrammerKR/ProXPL.git

# Navigate to directory
cd ProXPL
```

### Step 2: Create Build Directory

```bash
# Create build directory (isolates build artifacts)
mkdir build
cd build
```

### Step 3: Configure with CMake

#### Debug Build (for development)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

#### Release Build (optimized)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### Custom Compiler
```bash
# Use Clang
CC=clang cmake ..

# Use GCC
CC=gcc cmake ..

# Use MSVC (Windows)
cmake -G "Visual Studio 17 2022" ..
```

### Step 4: Build

#### Using CMake

```bash
# Build all targets
cmake --build .

# Build specific target
cmake --build . --target prox

# Build with verbosity
cmake --build . --verbose
```

#### Using Make (Unix/Linux/macOS)

```bash
# From build directory
make

# From root directory
cd ..
make
```

#### Using Visual Studio (Windows)

```bash
# From build directory
cmake --build . --config Release

# Or open in Visual Studio
start ProXPL.sln
```

### Step 5: Install (Optional)

```bash
# Install to system directories
cmake --install .

# Install to custom location
cmake --install . --prefix ~/.local

# On Windows
cmake --install . --prefix "C:\Program Files\ProXPL"
```

---

## Platform-Specific Guides

### Linux

#### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run
./prox ../examples/hello.prox
```

#### Fedora/RHEL

```bash
# Install dependencies
sudo dnf install -y gcc-c++ cmake make git

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run
./prox ../examples/hello.prox
```

#### Arch Linux

```bash
# Install dependencies
sudo pacman -S base-devel cmake git

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run
./prox ../examples/hello.prox
```

### macOS

```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install gcc cmake

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run
./prox ../examples/hello.prox

# Optional: Install globally
cmake --install . --prefix /usr/local
prox ../examples/hello.prox
```

### Windows (MSVC)

#### Using Command Prompt

```bash
# Install dependencies
# Download CMake from https://cmake.org/download/
# Install Visual Studio Community Edition

# Clone and build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build
cd build

# Configure
cmake -G "Visual Studio 17 2022" ..

# Build (Release)
cmake --build . --config Release

# Run
.\Release\prox.exe ..\examples\hello.prox
```

#### Using PowerShell

```powershell
# Install dependencies (using Chocolatey)
choco install cmake git visualstudio2022community

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build; cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
.\Release\prox.exe ..\examples\hello.prox
```

### Windows (MinGW)

```bash
# Install MinGW and CMake
# From MSYS2 shell:
pacman -S mingw-w64-ucrt-x86_64-gcc cmake

# Build
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
.\prox.exe ..\examples\hello.prox
```

---

## Build Configuration

### CMake Variables

Pass variables with `-DVARIABLE=VALUE`:

```bash
# Build type: Debug, Release, RelWithDebInfo, MinSizeRel
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compiler
cmake -DCMAKE_C_COMPILER=clang ..

# Enable tests
cmake -DBUILD_TESTS=ON ..

# Enable documentation
cmake -DBUILD_DOCS=ON ..

# Custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

### Compiler Flags

```bash
# With optimization and debug info
cmake -DCMAKE_C_FLAGS="-O2 -g" ..

# With address sanitizer (for debugging)
cmake -DCMAKE_C_FLAGS="-fsanitize=address -g" ..

# Maximum warnings (recommended for development)
cmake -DCMAKE_C_FLAGS="-Wall -Wextra -Werror" ..
```

### Build Variants

#### Development Build
```bash
mkdir build-dev && cd build-dev
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
cmake --build .
```

#### Production Build
```bash
mkdir build-prod && cd build-prod
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

#### Minimal Build
```bash
mkdir build-minimal && cd build-minimal
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_DOCS=OFF ..
cmake --build .
```

---

## Running Tests

### Test Compilation

```bash
# Enable tests during configuration
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
cmake --build .
```

### Running Tests

```bash
# Run all tests
ctest --verbose

# Run specific test
ctest -R test_lexer --verbose

# Run with output on failure
ctest --output-on-failure

# Run with timeout per test
ctest --timeout 5
```

### Test Coverage

```bash
# Generate coverage report
cmake -DENABLE_COVERAGE=ON ..
cmake --build .
ctest --verbose
gcov ../src/**/*.c

# Generate HTML report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
open coverage_html/index.html  # macOS
```

---

## Troubleshooting

### CMake Not Found

```bash
# Install CMake
# Linux: sudo apt-get install cmake
# macOS: brew install cmake
# Windows: Download from https://cmake.org/download/

# Verify installation
cmake --version
```

### Compiler Not Found

```bash
# Install compiler
# Linux: sudo apt-get install build-essential
# macOS: xcode-select --install
# Windows: Install Visual Studio or MinGW

# Specify compiler explicitly
CC=gcc cmake ..
```

### Build Fails with "command not found"

```bash
# Check if you're in the correct directory
pwd  # Should be ProXPL/build

# Verify CMake configuration
cmake ..

# Try clean rebuild
cd ..
rm -rf build
mkdir build && cd build
cmake ..
cmake --build .
```

### MSVC Build Fails

```bash
# Ensure Developer Command Prompt is used (not regular cmd)
# Open: "x64 Native Tools Command Prompt for VS 2022"

# Or set up environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# Try again
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### "Source file is not under the build directory"

```bash
# Wrong: Building in source directory
cd ProXPL
cmake .  # WRONG

# Right: Building in separate directory
cd ProXPL
mkdir build && cd build
cmake ..
cmake --build .
```

### Linker Errors

```bash
# Check for missing object files
cmake --build . --verbose

# Clear build cache and rebuild
cd ..
rm -rf build
mkdir build && cd build
cmake ..
cmake --build .
```

### Out of Memory During Build

```bash
# Use single-threaded build
cmake --build . -j 1

# Or limit threads
cmake --build . -j 2
```

---

## Advanced Options

### Parallel Build

```bash
# Use 4 CPU cores
cmake --build . -j 4

# Use all available cores
cmake --build . -j $(nproc)  # Linux/macOS
cmake --build . -j %NUMBER_OF_PROCESSORS%  # Windows
```

### Static Analysis

```bash
# Enable clang analyzer
cmake -DCMAKE_C_COMPILER=clang --analyzer ..

# Run clang-tidy
clang-tidy src/**/*.c -- -I./include

# Run cppcheck
cppcheck --enable=all src/
```

### Profiling Build

```bash
# With profiling support (gprof)
cmake -DCMAKE_C_FLAGS="-pg" ..
cmake --build .

# Run program and generate profile
./prox examples/hello.prox
gprof ./prox gmon.out
```

### Cross-Compilation

```bash
# For ARM (e.g., Raspberry Pi)
cmake -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=armv7l \
      -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc ..

# For WebAssembly
cmake -DCMAKE_TOOLCHAIN_FILE=Emscripten.cmake ..
```

### Custom Installation

```bash
# Install to custom directory
cmake --install . --prefix ~/.local

# Add to PATH
export PATH="$HOME/.local/bin:$PATH"

# Verify
which prox
prox --version
```

---

## Verification

### Post-Build Verification

```bash
# Check executable
./prox --version  # If implemented
file ./prox       # Check file type

# Run example
./prox ../examples/hello.prox

# Run REPL
./prox
# Type: print("Hello");
# Type: exit
```

### Performance Baseline

```bash
# Run fibonacci benchmark
time ./prox ../examples/fibonacci.prox

# Run with REPL
time ./prox examples/hello.prox

# Memory usage
ps aux | grep prox
```

---

## Next Steps

1. **Read Documentation**: See [README.md](../README.md)
2. **Run Examples**: Try `examples/*.prox` files
3. **Explore Code**: Check `src/` and `include/` directories
4. **Contribute**: See [CONTRIBUTING.md](../CONTRIBUTING.md)

---

## Support

Having trouble building? 

- Check GitHub Issues: https://github.com/ProgrammerKR/ProXPL/issues
- Open a new issue with:
  - OS and version
  - Compiler and version
  - CMake version
  - Error output
  - Steps to reproduce

---

**Happy Building!** 🚀
