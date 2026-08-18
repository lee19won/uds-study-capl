@echo off
REM ===========================================================================
REM build_mingw.bat
REM Builds SecurityAccessDLL.dll (GenerateKeyExOpt) with MinGW-w64 g++.
REM
REM Prerequisite: MinGW-w64 installed and "g++" available on PATH.
REM   Easiest install (if not already present): https://winlibs.com/
REM   (download the "UCRT runtime" x86_64 build, unzip anywhere, add its
REM   bin\ folder to PATH) - or via MSYS2 (https://www.msys2.org/):
REM     pacman -S mingw-w64-x86_64-gcc
REM   then run this script from an "MSYS2 MinGW x64" shell / normal cmd
REM   with that mingw64\bin folder on PATH.
REM
REM Usage: just double-click this file, or run it from cmd/PowerShell in
REM        this folder. Produces SecurityAccessDLL.dll (64-bit) next to
REM        this script.
REM ===========================================================================

where g++ >nul 2>nul
if errorlevel 1 (
    echo ERROR: g++ not found on PATH. Install MinGW-w64 first ^(see comment
    echo        at the top of this script^), then re-run.
    pause
    exit /b 1
)

g++ -shared -O2 -Wall ^
    -o SecurityAccessDLL.dll ^
    GenerateKeyExOpt.cpp ^
    -Wl,--out-implib,SecurityAccessDLL.lib

if errorlevel 1 (
    echo BUILD FAILED.
    pause
    exit /b 1
)

echo.
echo Build OK: SecurityAccessDLL.dll
pause
