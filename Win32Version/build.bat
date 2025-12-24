@echo off
REM Build script for Just a Sample Win32 Version
REM This script builds the project using CMake and Visual Studio

echo ====================================
echo Just a Sample - Win32 Build Script
echo ====================================
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
if not exist build (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure project
echo Configuring CMake project...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed
    echo.
    echo If you don't have Visual Studio 2022, you can try:
    echo   - Visual Studio 16 2019: cmake .. -G "Visual Studio 16 2019" -A x64
    echo   - Visual Studio 15 2017: cmake .. -G "Visual Studio 15 2017" -A x64
    echo   - MinGW: cmake .. -G "MinGW Makefiles"
    pause
    cd ..
    exit /b 1
)

REM Build project
echo.
echo Building project (Release)...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed
    pause
    cd ..
    exit /b 1
)

echo.
echo ====================================
echo Build completed successfully!
echo ====================================
echo.
echo Executable location: build\bin\Release\JustASample_Win32.exe
echo.
echo To run the application:
echo   cd build\bin\Release
echo   JustASample_Win32.exe
echo.

cd ..
pause
