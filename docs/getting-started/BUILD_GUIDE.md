# Build Guide

Build ProXPL from source on Windows, Linux, or macOS.

## Requirements

- C/C++ compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.15+
- LLVM 10+ (for AOT compilation)
- Git

## Quick Build

```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Platform-Specific

### Windows (Visual Studio)
```bash
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

### Linux
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```

### macOS
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## CLI via Node.js

Enhanced developer CLI with watch mode:
```bash
cd src/cli
npm install
npm link
```

## Testing

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
make
make test
```

---

**Status**: Stable
**See Also**: [README.md](../README.md), [Prm Guide](PRM.md)
