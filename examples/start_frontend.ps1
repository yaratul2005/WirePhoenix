# PhoenixWire Chat Frontend - Quick Start Launcher for PowerShell

Write-Host ""
Write-Host "PhoenixWire Chat - Web Frontend" -ForegroundColor Cyan
Write-Host "Quick Start Launcher" -ForegroundColor Cyan
Write-Host ""

$phpPath = $null
$port = 8000

# Check command line arguments
if ($args.Count -gt 0) {
    $port = [int]$args[0]
}

# Try to find PHP in common locations
$commonPaths = @(
    "C:\xampp\php\php.exe",
    "C:\php\php.exe",
    "C:\Program Files\PHP\php.exe"
)

foreach ($path in $commonPaths) {
    if (Test-Path $path) {
        $phpPath = $path
        Write-Host "[OK] Found PHP at: $path" -ForegroundColor Green
        break
    }
}

# If not found, try using 'php' from PATH
if (-not $phpPath) {
    Write-Host "[*] Searching for php in system PATH..." -ForegroundColor Yellow
    try {
        $phpPath = (Get-Command php -ErrorAction Stop).Source
        Write-Host "[OK] Found PHP: $phpPath" -ForegroundColor Green
    }
    catch {
        Write-Host "[ERROR] PHP not found!" -ForegroundColor Red
        Write-Host ""
        Write-Host "Please ensure PHP is installed and added to system PATH" -ForegroundColor Yellow
        exit 1
    }
}

Write-Host ""
Write-Host "Starting development server..." -ForegroundColor Cyan
Write-Host "=======================================" -ForegroundColor Gray
Write-Host ""
Write-Host "Open your browser to: http://localhost:$port" -ForegroundColor Green
Write-Host "Press Ctrl+C to stop the server" -ForegroundColor Yellow
Write-Host ""

# Start the server
& "$phpPath" -S localhost:$port serve.php
