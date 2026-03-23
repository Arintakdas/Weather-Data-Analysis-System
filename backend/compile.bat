@echo off
echo ========================================
echo Weather Data Analysis - Backend Compiler
echo ========================================
echo.

cd /d "%~dp0"

echo Compiling C backend...
echo.

:: --- CRITICAL FIX: Added file_io.c to the command ---
gcc main.c dsa.c file_io.c -o weather.exe -std=c11

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo SUCCESS! Backend compiled successfully.
    echo ========================================
    echo.
    echo Run the app using: weather.exe
    echo.
) else (
    echo.
    echo ========================================
    echo ERROR: Compilation failed!
    echo ========================================
    echo.
)

echo.
pause