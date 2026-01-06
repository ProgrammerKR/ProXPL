
Add-Type -AssemblyName System.Drawing
$source = "$PSScriptRoot\extension\logo.png"
$dest = "$PSScriptRoot\assets\icon.ico"

if (Test-Path $source) {
    try {
        $img = [System.Drawing.Bitmap]::FromFile($source)
        # Create a new bitmap with standard icon size if needed, but let's try direct conversion first
        # But for best quality, let's just use GetHicon
        $iconHandle = $img.GetHicon()
        $icon = [System.Drawing.Icon]::FromHandle($iconHandle)
        
        $fs = New-Object System.IO.FileStream($dest, [System.IO.FileMode]::Create)
        $icon.Save($fs)
        $fs.Close()
        
        $icon.Dispose() # Important to release handle
        $img.Dispose()
        
        Write-Host "Success: Created $dest"
    } catch {
        Write-Host "Error: $_"
    }
} else {
    Write-Host "Source file not found: $source"
}
