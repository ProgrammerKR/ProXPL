# build_asr_demo.ps1
# Attempts to compile the ASR demo with MSVC `cl` or `g++` if available.

$buildDir = "build"
if (-not (Test-Path $buildDir)) { New-Item -ItemType Directory -Path $buildDir | Out-Null }

Write-Host "Checking for MSVC 'cl'..."
$cl = Get-Command cl -ErrorAction SilentlyContinue
if ($cl) {
    Write-Host "Found cl: compiling with MSVC..."
    cl /EHsc /std:c++17 /O2 runtime\asr_lockfree_queue.cpp runtime\asr_supervisor.cpp tests\asr_queue_test.cpp /Fe:$buildDir\asr_test.exe
    exit $LASTEXITCODE
}

Write-Host "MSVC 'cl' not found; checking for 'g++'..."
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if ($gpp) {
    Write-Host "Found g++: compiling with g++..."
    g++ -std=c++17 -O2 -pthread runtime\asr_lockfree_queue.cpp runtime\asr_supervisor.cpp tests\asr_queue_test.cpp -o $buildDir\asr_test.exe
    exit $LASTEXITCODE
}

Write-Host "No supported C++ compiler found in PATH."
Write-Host "Options:"
Write-Host "  - Install Visual Studio Build Tools and run from 'Developer Command Prompt'"
Write-Host "  - Install MSYS2 / MinGW and add g++ to PATH"
exit 1
