#!/usr/bin/env pwsh
# --------------------------------------------------
#   Project: ProX Programming Language (ProXPL)
#   Author:  ProgrammerKR
#   Created: 2025-12-16
#   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

<#
  build_msvc.ps1
  Build the bench_simple.exe using MSVC `cl.exe`.

  Usage: run this inside a "Developer Command Prompt for VS" or after running
  `VsDevCmd.bat` so `cl.exe` and the toolchain are on PATH.
#>

if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    Write-Error "cl.exe not found in PATH. Run this script from 'Developer Command Prompt for VS' or run VsDevCmd.bat first."
    exit 1
}

$root = (Resolve-Path "$PSScriptRoot\..\..").Path
$include = Join-Path $root "include"
$src = Join-Path $root "src"
$bench = Join-Path $PSScriptRoot "bench_simple.c"
$files = @(
    '"' + $bench + '"',
    '"' + (Join-Path $src "vm\vm_dispatch.c") + '"',
    '"' + (Join-Path $src "vm\bytecode.c") + '"'
)

$incs = "/I""$include"" /I""$src"""
$clopts = "/nologo /O2 /W3 /std:c11"
$outfile = "bench_simple.exe"

$cmd = "cl.exe $clopts $incs /Fe:$outfile " + ($files -join ' ')
Write-Host "Running: $cmd"
Invoke-Expression $cmd
