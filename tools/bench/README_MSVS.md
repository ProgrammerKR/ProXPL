# Building `bench_simple` on Windows (MSVC)

Prerequisites
- Visual Studio (Build Tools) with C/C++ workload installed.

Steps
1. Open "Developer Command Prompt for VS" (or run `VsDevCmd.bat` for your VS installation).
2. From that prompt, run the PowerShell build script included here:

```powershell
cd tools/bench
pwsh .\build_msvc.ps1
```

Notes
- The build script expects `cl.exe` on PATH. If you get an error, ensure you launched the Developer Command Prompt.
- The script compiles `bench_simple.c` together with `src\vm\vm_dispatch.c` and `src\vm\bytecode.c` and produces `bench_simple.exe` in the same folder.
