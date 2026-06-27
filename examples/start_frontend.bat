@echo off
REM PhoenixWire Chat Frontend - Quick Start Launcher for Windows
REM This script finds PHP and starts the development server

echo.
echo ╔════════════════════════════════════════════╗
echo ║     PhoenixWire Chat - Web Frontend        ║
echo ║     Quick Start Launcher                   ║
echo ╚════════════════════════════════════════════╝
echo.

REM Determine PHP location
set PHP_PATH=
set PORT=8000

REM Check common PHP locations
if exist "C:\xampp\php\php.exe" (
    set PHP_PATH=C:\xampp\php\php.exe
    echo [Found] PHP at C:\xampp\php
) else if exist "C:\php\php.exe" (
    set PHP_PATH=C:\php\php.exe
    echo [Found] PHP at C:\php
) else (
    echo [Warning] PHP not found in common locations
    echo [Trying] Using php from PATH...
    set PHP_PATH=php
)

REM Check for port argument
if not "%1"=="" set PORT=%1

echo.
echo Starting development server on http://localhost:%PORT%
echo Press Ctrl+C to stop the server
echo.

REM Start server
"%PHP_PATH%" -S localhost:%PORT% serve.php

REM If server fails
if errorlevel 1 (
    echo.
    echo [Error] Failed to start PHP server
    echo [Help] Make sure PHP is installed and accessible
    echo [Help] You can manually run: php -S localhost:%PORT% serve.php
    pause
)
