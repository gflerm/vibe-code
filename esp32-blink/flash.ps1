$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot
try {
    Write-Host "Save all files in VS Code now (Ctrl+K, S)." -ForegroundColor Yellow
    Read-Host "Press Enter after the files have been saved"

    Write-Host "Building and flashing the ESP32-S3..." -ForegroundColor Cyan
    python -m platformio run --target upload
    if ($LASTEXITCODE -ne 0) {
        throw "PlatformIO build/upload failed with exit code $LASTEXITCODE."
    }
}
finally {
    Pop-Location
}
