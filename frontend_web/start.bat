@echo off
echo ========================================
echo Weather Data Analysis - Starting Server
echo ========================================
echo.

cd /d "%~dp0"

echo Checking if backend is compiled...
if not exist "..\backend\weather.exe" (
    echo.
    echo [WARNING] Backend not found! Attempting to compile...
    cd ..\backend
    call compile.bat
    cd ..\frontend_web
    
    if not exist "..\backend\weather.exe" (
        echo.
        echo [ERROR] Auto-compilation failed. Please fix errors in backend.
        pause
        exit /b 1
    )
    echo [SUCCESS] Backend compiled successfully.
)

echo Backend found!
echo.
echo Starting Node.js server...
echo The server will start on http://localhost:3000
echo.
echo Press Ctrl+C to stop the server.
echo ========================================
echo.

node server.js
