@echo off
REM MeshChat Build Script for Windows

echo 🔨 Building MeshChat Firmware...

REM Check if ESP-IDF is available
where idf.py >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ ESP-IDF not found. Please run:
    echo    %USERPROFILE%\esp\esp-idf\export.bat
    exit /b 1
)

REM Configure for ESP32-S3
echo 📋 Setting target to ESP32-S3...
idf.py set-target esp32s3

REM Build firmware
echo 🏗️  Building firmware...
idf.py build

echo ✅ Build complete!
echo.
echo To flash the firmware:
echo    idf.py -p COM3 flash monitor
echo.
echo To serve the web interface:
echo    cd web && python -m http.server 8000
