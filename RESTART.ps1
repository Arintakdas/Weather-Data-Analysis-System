# WEATHER APP RESTART SCRIPT
# RIGHT-CLICK THIS FILE -> "Run with PowerShell"

$ROOT = Split-Path -Parent $MyInvocation.MyCommand.Path
$BACKEND = Join-Path $ROOT "backend"
$FRONTEND = Join-Path $ROOT "frontend_web"

Write-Host "===========================================" -ForegroundColor Cyan
Write-Host " Weather Analytics - PowerShell Restart" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

# Step 1: Force kill whatever is on port 3000
Write-Host "[1/3] Killing process on port 3000..." -ForegroundColor Yellow
try {
    $port3000 = netstat -ano | findstr ":3000" | findstr "LISTENING"
    if ($port3000) {
        $pid3000 = ($port3000 -split '\s+')[-1]
        Stop-Process -Id $pid3000 -Force -ErrorAction SilentlyContinue
        Write-Host "      Killed PID $pid3000" -ForegroundColor Green
    } else {
        Write-Host "      Port 3000 is free." -ForegroundColor Green
    }
} catch {
    Write-Host "      No process to kill." -ForegroundColor Gray
}

# Also kill all node instances
Get-Process -Name "node" -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Seconds 1

# Step 2: Compile C
Write-Host "[2/3] Compiling C backend..." -ForegroundColor Yellow
Set-Location $BACKEND
$result = & gcc main.c dsa.c file_io.c -o weather.exe -std=c11 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "COMPILE FAILED: $result" -ForegroundColor Red
    Write-Host "Press Enter to exit..."
    Read-Host
    exit 1
}
Write-Host "      Compiled OK!" -ForegroundColor Green

# Step 3: Start Node
Write-Host "[3/3] Starting server at http://localhost:3000 ..." -ForegroundColor Yellow
Set-Location $FRONTEND

# Open browser after 2 seconds
Start-Job -ScriptBlock { Start-Sleep 2; Start-Process "http://localhost:3000" } | Out-Null

Write-Host ""
Write-Host "===========================================" -ForegroundColor Green
Write-Host " Server running! Keep this window open." -ForegroundColor Green
Write-Host " http://localhost:3000" -ForegroundColor Green
Write-Host " Press Ctrl+C to stop." -ForegroundColor Green
Write-Host "===========================================" -ForegroundColor Green
Write-Host ""

& node server.js
