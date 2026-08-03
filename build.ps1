$ErrorActionPreference = "Stop"

if (-not (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

Write-Host "`nBuild successful!" -ForegroundColor Green
Write-Host "Run Catherine files using:"
Write-Host "  .\build\Release\catherine.exe <filename.rine>  (MSVC)"
Write-Host "or"
Write-Host "  .\build\catherine.exe <filename.rine>          (MinGW / Ninja)"
