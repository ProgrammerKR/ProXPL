# scripts/setup_llvm_windows.ps1
# Robust LLVM detection and setup for Windows CI

function Resolve-LLVMLayout {
    $candidateRoots = New-Object System.Collections.Generic.List[string]
    
    # Check if llvm-config is already in PATH
    $llvmConfig = Get-Command llvm-config -ErrorAction SilentlyContinue
    if ($llvmConfig) {
        $binPath = $llvmConfig.Source
        $binDir = Split-Path $binPath
        $root = Split-Path $binDir
        $candidateRoots.Add($root)
    }

    # Common installation paths
    $candidateRoots.Add("C:\Program Files\LLVM")
    $candidateRoots.Add("C:\Program Files (x86)\LLVM")
    $candidateRoots.Add("C:\ProgramData\chocolatey\lib\llvm\tools\llvm")
    $candidateRoots.Add("C:\ProgramData\chocolatey\lib\llvm")
    
    if ($env:ChocolateyInstall) {
        $candidateRoots.Add((Join-Path $env:ChocolateyInstall "lib\llvm\tools\llvm"))
        $candidateRoots.Add((Join-Path $env:ChocolateyInstall "lib\llvm"))
    }

    $cmakeSuffixes = @(
        "lib\cmake\llvm",
        "lib\llvm\cmake",
        "share\llvm\cmake",
        "cmake"
    )

    foreach ($root in ($candidateRoots | Select-Object -Unique)) {
        if (-not $root -or -not (Test-Path $root)) { continue }
        
        Write-Host "Checking LLVM root candidate: $root"
        foreach ($suffix in $cmakeSuffixes) {
            $cmakeDir = Join-Path $root $suffix
            if (Test-Path (Join-Path $cmakeDir "LLVMConfig.cmake")) {
                return @{
                    Root = $root
                    CMakeDir = $cmakeDir
                    BinDir = Join-Path $root "bin"
                }
            }
        }
    }

    # Fallback: Recursive search in Program Files if still not found
    Write-Host "Performing exhaustive search for LLVMConfig.cmake..."
    $searchRoots = @("C:\Program Files", "C:\Program Files (x86)", "C:\ProgramData\chocolatey\lib")
    foreach ($sr in $searchRoots) {
        if (-not (Test-Path $sr)) { continue }
        $configFile = Get-ChildItem -Path $sr -Filter "LLVMConfig.cmake" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($configFile) {
            $cmakeDir = $configFile.DirectoryName
            Write-Host "Found LLVMConfig.cmake at: $cmakeDir"
            # Try to find the root by going up from cmake dir
            $root = $cmakeDir
            for ($i = 0; $i -lt 4; $i++) {
                if (Test-Path (Join-Path $root "bin")) { break }
                $parent = Split-Path -Parent $root
                if ($parent -eq $root) { break }
                $root = $parent
            }
            return @{
                Root = $root
                CMakeDir = $cmakeDir
                BinDir = Join-Path $root "bin"
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
    choco install llvm -y --version 17.0.6 --allow-downgrade --no-progress --limit-output
    
    # Refresh PATH for the current session
    Write-Host "Refreshing PATH..."
    $env:PATH = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    
    $layout = Resolve-LLVMLayout
}

# 3. Export findings
if ($layout) {
    $llvmRoot = $layout.Root -replace "\\", "/"
    $cmakeDir = $layout.CMakeDir -replace "\\", "/"
    $binDir = $layout.BinDir

    Write-Host "Resolved LLVM root: $llvmRoot"
    Write-Host "Resolved LLVM_DIR: $cmakeDir"

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
    Write-Host "DUMPING COMMON LOCATIONS FOR DEBUG:"
    Get-ChildItem "C:\Program Files" -ErrorAction SilentlyContinue | Select-Object Name
    Get-ChildItem "C:\ProgramData\chocolatey\lib" -ErrorAction SilentlyContinue | Select-Object Name
    Write-Error "Failed to detect LLVM after installation."
    exit 1
}
