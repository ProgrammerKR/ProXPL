# scripts/setup_llvm_windows.ps1
# Robust LLVM detection and setup for Windows CI

function Resolve-LLVMLayout {
    $candidateRoots = New-Object System.Collections.Generic.List[string]
    
    # 1. Direct LLVM_ROOT or LLVM_DIR environment variables
    if ($env:LLVM_ROOT -and (Test-Path $env:LLVM_ROOT)) { $candidateRoots.Add($env:LLVM_ROOT) }
    if ($env:LLVM_DIR -and (Test-Path $env:LLVM_DIR)) {
        $dir = $env:LLVM_DIR
        $candidateRoots.Add($dir)
        # Try to backtrack to root if LLVM_DIR points to cmake dir
        $candidateRoots.Add((Split-Path (Split-Path (Split-Path $dir)))) # lib/cmake/llvm -> root
        $candidateRoots.Add((Split-Path (Split-Path $dir)))              # cmake -> root
    }

    # 2. Check known paths for llvm-config.exe or clang.exe explicitly
    $checkFiles = @(
        "bin\llvm-config.exe",
        "bin\clang.exe",
        "bin\lld.exe"
    )
    $searchBases = @(
        "C:\LLVM",
        "C:\Program Files\LLVM",
        "C:\ProgramData\chocolatey\lib\llvm\tools\llvm",
        "C:\tools\llvm"
    )
    
    foreach ($base in $searchBases) {
        if (-not (Test-Path $base)) { continue }
        foreach ($file in $checkFiles) {
            $p = Join-Path $base $file
            if (Test-Path $p) {
                Write-Host "Found LLVM component at: $p"
                $root = $base
                $binDir = Join-Path $root "bin"
                
                # Try to find cmake dir via llvm-config if it exists
                if ($p -like "*llvm-config.exe") {
                    $cmakeDir = & $p --cmakedir 2>$null
                    if ($cmakeDir -and (Test-Path $cmakeDir)) {
                        return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = $binDir }
                    }
                }
                
                # Otherwise search for LLVMConfig.cmake in this root
                $cmakeSuffixes = @("lib\cmake\llvm", "lib\llvm\cmake", "share\llvm\cmake", "cmake", "lib\cmake", "share\cmake\llvm")
                foreach ($suffix in $cmakeSuffixes) {
                    $cmakeDir = Join-Path $root $suffix
                    if (Test-Path (Join-Path $cmakeDir "LLVMConfig.cmake")) {
                        return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = $binDir }
                    }
                    if (Test-Path (Join-Path $cmakeDir "llvm-config.cmake")) {
                        return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = $binDir }
                    }
                }
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

    # 4. Aggressive search for LLVMConfig.cmake or llvm-config.cmake in candidate roots
    $candidateRoots.Add("C:\LLVM")
    $candidateRoots.Add("C:\Program Files\LLVM")
    $candidateRoots.Add("C:\ProgramData\chocolatey")
    
    $uniqueRoots = $candidateRoots | Where-Object { $_ -ne $null -and $_ -ne "" -and (Test-Path $_) } | Select-Object -Unique
    foreach ($root in $uniqueRoots) {
        Write-Host "Aggressively searching for LLVM CMake files in: $root"
        $configFile = Get-ChildItem -Path $root -Filter "*LLVMConfig.cmake" -Recurse -Depth 10 -ErrorAction SilentlyContinue | Select-Object -First 1
        if (-not $configFile) {
            $configFile = Get-ChildItem -Path $root -Filter "*llvm-config.cmake" -Recurse -Depth 10 -ErrorAction SilentlyContinue | Select-Object -First 1
        }
        
        if ($configFile) {
            $cmakeDir = $configFile.DirectoryName
            Write-Host "Found config file at: $($configFile.FullName)"
            # Try to find root by looking for 'bin' up the tree
            $current = $cmakeDir
            for ($i = 0; $i -lt 5; $i++) {
                if (Test-Path (Join-Path $current "bin")) {
                    return @{ Root = $current; CMakeDir = $cmakeDir; BinDir = Join-Path $current "bin" }
                }
                $parent = Split-Path $current -Parent
                if ($parent -eq $current) { break }
                $current = $parent
            }
            # Fallback to current if bin not found
            return @{ Root = $root; CMakeDir = $cmakeDir; BinDir = Join-Path $root "bin" }
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
            
            $binPath = Join-Path $dp "bin"
            if (Test-Path $binPath) {
                Write-Host "`nContents of $binPath` (first 10 items):"
                Get-ChildItem $binPath -ErrorAction SilentlyContinue | Select-Object Name | Select-Object -First 10 | Format-Table -HideTableHeaders
            }
            
            $libPath = Join-Path $dp "lib"
            if (Test-Path $libPath) {
                Write-Host "`nContents of $libPath` (first 10 items):"
                Get-ChildItem $libPath -ErrorAction SilentlyContinue | Select-Object Name | Select-Object -First 10 | Format-Table -HideTableHeaders
            }
        }
    }
    Write-Error "Failed to detect LLVM after installation."
    exit 1
}



