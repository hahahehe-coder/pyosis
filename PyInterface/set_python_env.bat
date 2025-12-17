@echo off
chcp 65001 >nul
title Set Python Environment Variables

:: Get the parent directory of parent directory (two levels up)
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"
cd ..
cd ..
set "BASE_DIR=%cd%"
set "PYTHON_PATH=%BASE_DIR%\Resource\python311"

echo Auto-detected Python directory: %PYTHON_PATH%
echo.

:: Check if path exists
if not exist "%PYTHON_PATH%" (
    echo Error: Python directory does not exist - %PYTHON_PATH%
    echo Please ensure python311 directory exists in: %BASE_DIR%
    pause
    exit /b 1
)

:: Set PYTHONHOME variable
echo Setting PYTHONHOME...
setx PYTHONHOME "%PYTHON_PATH%"

:: Safely update PATH without overwriting
echo Updating PATH...
for /f "tokens=2*" %%A in ('reg query HKCU\Environment /v PATH 2^>nul') do set "CURRENT_PATH=%%B"

:: Store to variable first to check
if "%CURRENT_PATH%"=="" (
    echo WARNING: PATH is empty, setting to Python directories only
    set "NEW_PATH=%PYTHON_PATH%;%PYTHON_PATH%\Scripts"
) else (
    :: Check if already contains Python
    echo %CURRENT_PATH% | find /i "%PYTHON_PATH%" >nul
    if errorlevel 1 (
        :: Add Python to of PATH
        set "NEW_PATH=%CURRENT_PATH%;%%PYTHONHOME%%;%%PYTHONHOME%%\Scripts"
    ) else (
        echo Python already in PATH
        set "NEW_PATH=%CURRENT_PATH%"
    )
)

:: Show what will be set
echo New PATH will be: %NEW_PATH%

:: Set the new PATH
setx PATH "%NEW_PATH%"

:: Set PYTHONIOENCODING
echo Setting PYTHONIOENCODING...
setx PYTHONIOENCODING "utf-8"

echo.
echo ====================================
echo Environment variables set successfully!
echo.
echo Variables set:
echo   PYTHONHOME=%PYTHON_PATH%
echo   PATH=%PYTHON_PATH%;%PYTHON_PATH%\Scripts;...
echo   PYTHONIOENCODING=utf-8
echo.
echo Important: 
echo 1. Restart command prompt for changes to take effect
echo 2. Test with:
echo      python --version
echo      echo %%PYTHONIOENCODING%%
echo ====================================
pause