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

    # 2. Check if llvm-config is in PATH
    $llvmConfig = Get-Command llvm-config -ErrorAction SilentlyContinue
    if ($llvmConfig) {
        $binDir = Split-Path $llvmConfig.Source
        $root = Split-Path $binDir
        $candidateRoots.Add($root)
    }

    # 3. Known installation paths
    $candidateRoots.Add("C:\LLVM")
    $candidateRoots.Add("C:\Program Files\LLVM")
    $candidateRoots.Add("C:\Program Files (x86)\LLVM")
    $candidateRoots.Add("C:\ProgramData\chocolatey\lib\llvm\tools\llvm")
    $candidateRoots.Add("C:\ProgramData\chocolatey\lib\llvm")
    $candidateRoots.Add("C:\tools\llvm")
    
    if ($env:ChocolateyInstall) {
        $candidateRoots.Add((Join-Path $env:ChocolateyInstall "lib\llvm\tools\llvm"))
        $candidateRoots.Add((Join-Path $env:ChocolateyInstall "lib\llvm"))
    }

    $cmakeSuffixes = @(
        "lib\cmake\llvm",
        "lib\llvm\cmake",
        "share\llvm\cmake",
        "cmake",
        "lib\cmake"
    )

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

    # 4. Fallback: Recursive search in common areas with INCREASED DEPTH
    Write-Host "Performing exhaustive search for LLVMConfig.cmake (Depth 8)..."
    $searchRoots = @("C:\LLVM", "C:\Program Files", "C:\Program Files (x86)", "C:\ProgramData\chocolatey", "C:\tools")
    foreach ($sr in $searchRoots) {
        if (-not (Test-Path $sr)) { continue }
        # Depth 8 ensures we find it even in deep Chocolatey paths
        $configFile = Get-ChildItem -Path $sr -Filter "LLVMConfig.cmake" -Recurse -Depth 8 -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($configFile) {
            $cmakeDir = $configFile.DirectoryName
            Write-Host "Found LLVMConfig.cmake at: $cmakeDir"
            
            # Try to find the root by looking for 'bin' nearby
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
    Write-Host "Listing candidate root contents for diagnostics:"
    $debugPaths = @("C:\LLVM", "C:\Program Files\LLVM", "C:\ProgramData\chocolatey\lib\llvm")
    foreach ($dp in $debugPaths) {
        if (Test-Path $dp) {
            Write-Host "`nContents of $dp`:"
            Get-ChildItem $dp -Recurse -Depth 2 -ErrorAction SilentlyContinue | Select-Object FullName | Format-Table -HideTableHeaders
        }
    }
    
    Write-Error "Failed to detect LLVM after installation."
    exit 1
}


