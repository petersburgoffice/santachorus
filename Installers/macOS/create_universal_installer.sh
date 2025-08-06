#!/bin/bash

# Create Universal Binary installer for Saturos Pro
# This installer will work on both Intel and Apple Silicon Macs

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/Builds_Universal"

# Get version
VERSION=$(cat "$PROJECT_ROOT/version.txt" 2>/dev/null || echo "0.9.9")

echo "🔧 Creating Universal Binary installer for Saturos Pro v$VERSION..."

# Check if Universal build exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "❌ Universal build not found. Please run ./build_universal.sh first"
    exit 1
fi

# Find built plugins
AU_PLUGIN=$(find "$BUILD_DIR" -name "*.component" -type d | head -1)
VST3_PLUGIN=$(find "$BUILD_DIR" -name "*.vst3" -type d | head -1)

if [ -z "$AU_PLUGIN" ] || [ -z "$VST3_PLUGIN" ]; then
    echo "❌ Could not find built plugins in $BUILD_DIR"
    exit 1
fi

echo "📦 Found plugins:"
echo "   AU: $AU_PLUGIN"
echo "   VST3: $VST3_PLUGIN"

# Verify architectures
echo ""
echo "🔍 Verifying Universal Binary architectures:"
echo "AU Component:"
lipo -info "$AU_PLUGIN/Contents/MacOS/"* | head -1
echo "VST3 Plugin:"
lipo -info "$VST3_PLUGIN/Contents/MacOS/"* | head -1

# Create temporary directory for installer files
TEMP_DIR=$(mktemp -d)
INSTALLER_ROOT="$TEMP_DIR/installer_root"
mkdir -p "$INSTALLER_ROOT/Library/Audio/Plug-Ins/Components"
mkdir -p "$INSTALLER_ROOT/Library/Audio/Plug-Ins/VST3"

# Copy plugins to installer root
echo ""
echo "📋 Copying plugins to installer package..."
cp -R "$AU_PLUGIN" "$INSTALLER_ROOT/Library/Audio/Plug-Ins/Components/"
cp -R "$VST3_PLUGIN" "$INSTALLER_ROOT/Library/Audio/Plug-Ins/VST3/"

# Create installer package
INSTALLER_NAME="Santa_Chorus_${VERSION}_Universal_macOS_Installer.pkg"
INSTALLER_PATH="$SCRIPT_DIR/$INSTALLER_NAME"

echo "🔨 Building installer package..."
pkgbuild --root "$INSTALLER_ROOT" \
         --identifier "com.sonicmakers.santachorus.universal" \
         --version "$VERSION" \
         --install-location "/" \
         "$INSTALLER_PATH"

# Clean up
rm -rf "$TEMP_DIR"

if [ -f "$INSTALLER_PATH" ]; then
    INSTALLER_SIZE=$(du -h "$INSTALLER_PATH" | cut -f1)
    echo ""
    echo "✅ Universal Binary installer created successfully!"
    echo "📦 File: $INSTALLER_PATH"
    echo "📏 Size: $INSTALLER_SIZE"
    echo ""
    echo "🎯 This installer works on both Intel and Apple Silicon Macs"
    echo "🚀 Ready for distribution!"
else
    echo "❌ Failed to create installer"
    exit 1
fi 