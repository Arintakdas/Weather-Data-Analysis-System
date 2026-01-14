@echo off
echo ========================================
echo Weather Data Analysis - Backend Compiler
echo ========================================
echo.

cd /d "%~dp0"

echo Compiling C backend...
echo.

if not exist "weather_data_saved.csv" (
    echo [WARNING] weather_data.csv not found in backend directory!
    echo The application needs this file to load data.
    echo.
)

gcc -o weather.exe main.c dsa.c -std=c11

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo SUCCESS! Backend compiled successfully.
    echo ========================================
    echo.
    echo Testing backend...
    echo.
    weather.exe dashboard
    echo.
    echo ========================================
    echo Backend is ready to use!
    echo ========================================
) else (
    echo.
    echo ========================================
    echo ERROR: Compilation failed!
    echo ========================================
    echo.
    echo Make sure GCC is installed and in your PATH.
    echo Download MinGW from: https://www.mingw-w64.org/
    echo.
)

echo.
pause
