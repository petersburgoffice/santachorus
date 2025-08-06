@echo off
echo.
echo 🎵 Creating Saturos Pro Windows Installer...
echo.

REM Check if NSIS is installed
where makensis >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ Error: NSIS (Nullsoft Scriptable Install System) is not installed or not in PATH
    echo.
    echo 💡 Please install NSIS from: https://nsis.sourceforge.io/Download
    echo    Make sure to add NSIS to your system PATH during installation
    echo.
    pause
    exit /b 1
)

REM Check if VST3 plugin exists
if not exist "..\..\build\SaturVST_artefacts\VST3\Saturos Pro.vst3" (
    echo ❌ Error: VST3 plugin not found
    echo 💡 Please build the plugin first using the build scripts
    echo.
    pause
    exit /b 1
)

REM Get version from version.txt
set /p VERSION=<..\..\version.txt
echo 📦 Version: %VERSION%
echo.

REM Update version in NSIS script
powershell -Command "(gc santa_chorus_installer.nsi) -replace '!define PRODUCT_VERSION \".*\"', '!define PRODUCT_VERSION \"%VERSION%\"' | Out-File -encoding ASCII santa_chorus_installer_temp.nsi"

echo 🔨 Building Windows installer...
makensis santa_chorus_installer_temp.nsi

if %errorlevel% equ 0 (
    echo.
    echo ✅ Windows Installer created successfully!
    echo 📦 File: Santa_Chorus_%VERSION%_Windows_Installer.exe
    
    REM Get file size
    for %%A in ("Santa_Chorus_%VERSION%_Windows_Installer.exe") do (
        set size=%%~zA
        set /a sizeMB=!size!/1024/1024
    )
    echo 📏 Size: !sizeMB! MB
    echo.
    echo 💡 Test the installer before distribution!
) else (
    echo.
    echo ❌ Error: Failed to create installer
)

REM Clean up temporary file
del santa_chorus_installer_temp.nsi 2>nul

echo.
pause 