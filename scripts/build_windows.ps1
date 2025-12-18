# --------------------------------------------------
#   Project: ProX Programming Language (ProXPL)
#   Author:  ProgrammerKR
#   Created: 2025-12-16
#   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

param(
    [string]$BuildDir = "$PSScriptRoot\..\build"
)

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found. Install CMake or use Developer Command Prompt."
    exit 1
}

New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
Push-Location $BuildDir
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
Pop-Location

Write-Host "Build finished in $BuildDir"
