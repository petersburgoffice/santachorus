#!/bin/bash

# Build script for Universal Binary (Intel + Apple Silicon)
# This creates a single version compatible with both architectures

echo "🔧 Building Saturos Pro Universal Binary..."

# Increment version
if [ -f ./increment_version.sh ]; then
    ./increment_version.sh
fi

# Clean previous builds
rm -rf Builds_Universal
mkdir -p Builds_Universal

# Configure CMake for Universal Binary (both x86_64 and arm64)
cmake -B Builds_Universal \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
    .

# Build the project
cmake --build Builds_Universal --config Release --parallel 4

if [ $? -eq 0 ]; then
    echo "✅ Universal Binary build completed successfully!"
    echo ""
    echo "📦 Built plugins:"
    AU_PATH=$(find Builds_Universal -name "*.component" -type d)
    VST3_PATH=$(find Builds_Universal -name "*.vst3" -type d)
    echo "   AU: $AU_PATH"
    echo "   VST3: $VST3_PATH"
    echo ""
    
    # Verify architectures
    if [ -n "$AU_PATH" ]; then
        echo "🔍 AU Binary architectures:"
        lipo -info "$AU_PATH/Contents/MacOS/"*
    fi
    
    if [ -n "$VST3_PATH" ]; then
        echo "🔍 VST3 Binary architectures:"
        lipo -info "$VST3_PATH/Contents/MacOS/"*
    fi
    
    echo ""
    echo "🎯 To install Universal plugins manually:"
    echo "   cp -r \"$AU_PATH\" ~/Library/Audio/Plug-Ins/Components/"
    echo "   cp -r \"$VST3_PATH\" ~/Library/Audio/Plug-Ins/VST3/"
else
    echo "❌ Universal Binary build failed!"
    exit 1
fi 