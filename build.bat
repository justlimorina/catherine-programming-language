@echo off
if not exist build mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b %ERRORLEVEL%
)
cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo.
echo Build successful!
echo Run Catherine files using:
echo   .\build\Release\catherine.exe ^<filename.rine^>  (MSVC)
echo or
echo   .\build\catherine.exe ^<filename.rine^>          (MinGW / Ninja)
