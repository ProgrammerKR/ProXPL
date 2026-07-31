$status = git status --short
foreach ($line in $status) {
    if ($line.Trim() -ne "") {
        $file = $line.Substring(3).Trim()
        
        # Determine commit message based on file name
        if ($file.EndsWith(".cpp") -or $file.EndsWith(".java") -or $file.EndsWith(".go") -or $file.EndsWith(".js") -or $file.EndsWith(".py")) {
            $msg = "Add missing reference implementation: $file"
        } elseif ($file.EndsWith(".prox")) {
            $msg = "Add ProXPL feature tests: $file"
        } elseif ($file.Contains("logs/")) {
            $msg = "Clean up redundant log file: $file"
            git rm -q $file
        } elseif ($file -eq "CMakeLists.txt" -or $file -eq "include/common.h" -or $file -eq "SPEC.md") {
            $msg = "Bump version to 1.5.1 in $file"
        } elseif ($file -eq "compile_tests.bat") {
            $msg = "Update compile_tests.bat to use CMake"
        } elseif ($file -eq "docs/releases/v1.5.1.md") {
            $msg = "Add release notes for v1.5.1"
        } else {
            $msg = "Update $file"
        }
        
        Write-Host "Committing: $msg"
        git add $file
        git commit -m "$msg"
    }
}
Write-Host "Pushing..."
git push
