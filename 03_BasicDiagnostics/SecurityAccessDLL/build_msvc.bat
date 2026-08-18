@echo off
REM ===========================================================================
REM build_msvc.bat
REM Builds SecurityAccessDLL.dll (GenerateKeyExOpt) with the MSVC compiler.
REM
REM Prerequisite: Visual Studio (any edition) or the free "Build Tools for
REM Visual Studio" (https://visualstudio.microsoft.com/downloads/ ->
REM "Tools for Visual Studio" -> Build Tools), with the "Desktop development
REM with C++" workload.
REM
REM Run this from an "x64 Native Tools Command Prompt for VS" (Start Menu,
REM under the Visual Studio folder) so that "cl.exe" is on PATH - a plain
REM cmd/PowerShell window will NOT have cl.exe available.
REM ===========================================================================

where cl >nul 2>nul
if errorlevel 1 (
    echo ERROR: cl.exe not found on PATH.
    echo Run this script from an "x64 Native Tools Command Prompt for VS",
    echo not a plain cmd/PowerShell window.
    pause
    exit /b 1
)

cl /LD /EHsc /O2 /W4 GenerateKeyExOpt.cpp /Fe:SecurityAccessDLL.dll

if errorlevel 1 (
    echo BUILD FAILED.
    pause
    exit /b 1
)

echo.
echo Build OK: SecurityAccessDLL.dll
pause
