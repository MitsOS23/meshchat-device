#!/bin/bash
# MeshChat Build Script

set -e

echo "🔨 Building MeshChat Firmware..."

# Check if ESP-IDF is sourced
if [ -z "$IDF_PATH" ]; then
    echo "❌ ESP-IDF not found. Please run:"
    echo "   source \$HOME/esp/esp-idf/export.sh"
    exit 1
fi

# Configure for ESP32-S3
echo "📋 Setting target to ESP32-S3..."
idf.py set-target esp32s3

# Build firmware
echo "🏗️  Building firmware..."
idf.py build

echo "✅ Build complete!"
echo ""
echo "To flash the firmware:"
echo "   idf.py -p /dev/ttyUSB0 flash monitor"
echo ""
echo "To serve the web interface:"
echo "   cd web && python -m http.server 8000"
