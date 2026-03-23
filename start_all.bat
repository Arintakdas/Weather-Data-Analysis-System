@echo off
title Weather Analytics - RECOMPILE AND RESTART
color 0A
setlocal

set "ROOT=%~dp0"
set "BACKEND=%ROOT%backend"
set "FRONTEND=%ROOT%frontend_web"

cls
echo.
echo  =====================================================
echo   WEATHER ANALYTICS PRO - FRESH COMPILE + RESTART
echo  =====================================================
echo.
echo  This will take 30-60 seconds. Please wait...
echo.

:: Kill ALL node processes first
echo  [1/3] Stopping old server...
taskkill /F /IM node.exe >nul 2>&1
ping 127.0.0.1 -n 2 >nul

:: Compile
echo  [2/3] Compiling C backend with all latest fixes...
cd /d "%BACKEND%"
gcc main.c dsa.c file_io.c -o weather.exe -std=c11

if errorlevel 1 (
    color 4F
    echo.
    echo  *** COMPILE FAILED! Check that GCC is installed. ***
    echo  Run:  gcc --version
    echo.
    pause
    exit /b 1
)

echo  [2/3] Compiled OK!
echo.

:: Start server and open browser
echo  [3/3] Starting server...
cd /d "%FRONTEND%"

:: Launch browser after 3 seconds in background
start "" /b cmd /c "ping 127.0.0.1 -n 4 >nul && start http://localhost:3000"

echo.
echo  =====================================================
echo   SERVER IS RUNNING at http://localhost:3000
echo   DO NOT close this window!
echo  =====================================================
echo.
echo  Date search tip: Use YYYY-MM-DD (e.g. 2025-06-12)
echo  OR use DD-MM-YYYY (e.g. 12-06-2025) - both work!
echo.

node server.js
