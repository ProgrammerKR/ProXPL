# scripts/setup_llvm_windows.ps1
# Robust LLVM detection and setup for Windows CI

function Resolve-LLVMLayout {
    $candidateRoots = New-Object System.Collections.Generic.List[string]
    
    # 1. Direct LLVM_ROOT or LLVM_DIR environment variables
    if ($env:LLVM_ROOT -and (Test-Path $env:LLVM_ROOT)) { $candidateRoots.Add($env:LLVM_ROOT) }
    if ($env:LLVM_DIR -and (Test-Path $env:LLVM_DIR)) {
        $dir = $env:LLVM_DIR
        $candidateRoots.Add((Split-Path (Split-Path (Split-Path $dir)))) # lib/cmake/llvm -> root
        $candidateRoots.Add((Split-Path (Split-Path $dir)))              # cmake -> root
    }

    # 2. Check known paths for llvm-config.exe explicitly
    $llvmConfigPaths = @(
        "C:\LLVM\bin\llvm-config.exe",
        "C:\Program Files\LLVM\bin\llvm-config.exe",
        "C:\ProgramData\chocolatey\lib\llvm\tools\llvm\bin\llvm-config.exe"
    )
    foreach ($p in $llvmConfigPaths) {
        if (Test-Path $p) {
            Write-Host "Found llvm-config at: $p"
            $root = Split-Path (Split-Path $p)
            $cmakeDir = & $p --cmakedir 2>$null
            if ($cmakeDir -and (Test-Path $cmakeDir)) {
                return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = Join-Path $root "bin" }
            }
        }
    }

    # 3. Check if llvm-config is in PATH
    $llvmConfig = Get-Command llvm-config -ErrorAction SilentlyContinue
    if ($llvmConfig) {
        $binDir = Split-Path $llvmConfig.Source
        $root = Split-Path $binDir
        $cmakeDir = & $llvmConfig.Source --cmakedir 2>$null
        if ($cmakeDir -and (Test-Path $cmakeDir)) {
            return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = $binDir }
        }
    }

    # 4. Known installation paths with suffix search
    $candidateRoots.Add("C:\LLVM")
    $candidateRoots.Add("C:\Program Files\LLVM")
    $candidateRoots.Add("C:\Program Files (x86)\LLVM")
    $candidateRoots.Add("C:\ProgramData\chocolatey\lib\llvm\tools\llvm")
    $candidateRoots.Add("C:\tools\llvm")
    
    $cmakeSuffixes = @("lib\cmake\llvm", "lib\llvm\cmake", "share\llvm\cmake", "cmake", "lib\cmake")
    $uniqueRoots = $candidateRoots | Where-Object { $_ -ne $null -and $_ -ne "" } | Select-Object -Unique
    
    foreach ($root in $uniqueRoots) {
        if (-not (Test-Path $root)) { continue }
        Write-Host "Checking LLVM root candidate: $root"
        foreach ($suffix in $cmakeSuffixes) {
            $cmakeDir = Join-Path $root $suffix
            if (Test-Path (Join-Path $cmakeDir "LLVMConfig.cmake")) {
                return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = Join-Path $root "bin" }
            }
        }
    }

    # 5. Deep exhaustive search in C:\LLVM specifically
    if (Test-Path "C:\LLVM") {
        Write-Host "Performing deep search in C:\LLVM..."
        $configFile = Get-ChildItem -Path "C:\LLVM" -Filter "LLVMConfig.cmake" -Recurse -Depth 10 -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($configFile) {
            $cmakeDir = $configFile.DirectoryName
            $root = "C:\LLVM"
            return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = Join-Path $root "bin" }
        }
    }

    # 6. Fallback: Recursive search in common areas
    Write-Host "Performing exhaustive search for LLVMConfig.cmake (Depth 8)..."
    $searchRoots = @("C:\Program Files", "C:\Program Files (x86)", "C:\ProgramData\chocolatey", "C:\tools")
    foreach ($sr in $searchRoots) {
        if (-not (Test-Path $sr)) { continue }
        $configFile = Get-ChildItem -Path $sr -Filter "LLVMConfig.cmake" -Recurse -Depth 8 -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($configFile) {
            $cmakeDir = $configFile.DirectoryName
            Write-Host "Found LLVMConfig.cmake at: $cmakeDir"
            $root = $cmakeDir
            for ($i = 0; $i -lt 5; $i++) {
                if (Test-Path (Join-Path $root "bin")) { 
                    return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = Join-Path $root "bin" }
                }
                $parent = Split-Path -Parent $root
                if ($parent -eq $root) { break }
                $root = $parent
            }
        }
    }
    return $null
}

# 1. Initial check
$layout = Resolve-LLVMLayout

# 2. Install if not found
if (-not $layout) {
    Write-Host "LLVM not initially found. Installing via Chocolatey..."
    # Attempt install with BOTH package params and install arguments for maximum compatibility
    choco install llvm -y --version 17.0.6 --allow-downgrade --no-progress --limit-output --package-parameters="'/InstallDir:C:\LLVM'" --install-arguments='/D=C:\LLVM'
    
    # Refresh PATH for the current session
    Write-Host "Refreshing PATH..."
    $env:PATH = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    
    # Retry detection loop
    for ($i = 1; $i -le 3; $i++) {
        Write-Host "Retrying LLVM detection (Attempt $i/3)..."
        $layout = Resolve-LLVMLayout
        if ($layout) { break }
        if ($i -lt 3) {
            Write-Host "Waiting 5 seconds before next attempt..."
            Start-Sleep -Seconds 5
        }
    }
}

# 3. Export findings
if ($layout) {
    $llvmRoot = $layout.Root -replace "\\", "/"
    $cmakeDir = $layout.CMakeDir -replace "\\", "/"
    $binDir = $layout.BinDir

    Write-Host "`nResolved LLVM root: $llvmRoot"
    Write-Host "Resolved LLVM_DIR: $cmakeDir"
    Write-Host "Resolved BinDir: $binDir"

    if ($env:GITHUB_ENV) {
        Add-Content $env:GITHUB_ENV "LLVM_DIR=$cmakeDir"
        Add-Content $env:GITHUB_ENV "LLVM_ROOT=$llvmRoot"
        Add-Content $env:GITHUB_ENV "CMAKE_PREFIX_PATH=$llvmRoot"
    }
    if ($env:GITHUB_PATH -and (Test-Path $binDir)) {
        Write-Host "Adding to GITHUB_PATH: $binDir"
        Add-Content $env:GITHUB_PATH $binDir
    }
} else {
    Write-Host "`n--- DEBUG INFO: SEARCH FAILED ---"
    Write-Host "Top-level directory structure of candidates:"
    foreach ($dp in @("C:\LLVM", "C:\Program Files\LLVM", "C:\ProgramData\chocolatey\lib\llvm")) {
        if (Test-Path $dp) {
            Write-Host "`nContents of $dp`:"
            Get-ChildItem $dp -ErrorAction SilentlyContinue | Select-Object Name, Mode | Format-Table -HideTableHeaders
        }
    }
    Write-Error "Failed to detect LLVM after installation."
    exit 1
}



