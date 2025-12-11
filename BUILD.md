# Building ProXPL C Implementation (Manual)

If you don't have CMake installed, you can build manually with GCC:

## Install MinGW-w64 (if not installed)
1. Download from: https://winlibs.com/
2. Get the latest GCC build (e.g., `winlibs-x86_64-posix-seh-gcc-XX.X.X-mingw-w64ucrt-XX.X.X-r1.zip`)
3. Extract to `C:\mingw64`
4. Add `C:\mingw64\bin` to System PATH
5. Restart terminal and verify: `gcc --version`

## Build Commands

```bash
# Windows PowerShell or CMD
gcc -o prox.exe src/main.c src/vm/value.c src/vm/memory.c src/vm/chunk.c src/vm/debug.c src/vm/vm.c src/compiler/scanner.c src/compiler/compiler.c -Iinclude -std=c99 -Wall -g

# If successful, run:
.\prox.exe test.prox

# Or start REPL:
.\prox.exe
```

## Test
```bash
# Should output:
# -0.821429
# 70
# 4
# true
# false
# null
```
