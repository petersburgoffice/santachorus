#!/bin/bash

# Build script for Intel x86_64 architecture
# This creates a version compatible with Intel Macs

echo "🔧 Building Saturos Pro for Intel x86_64..."

# Clean previous builds
rm -rf Builds_Intel
mkdir -p Builds_Intel

# Configure CMake for x86_64 architecture
cmake -B Builds_Intel \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
    .

# Build the project
cmake --build Builds_Intel --config Release --parallel 4

if [ $? -eq 0 ]; then
    echo "✅ Intel build completed successfully!"
    echo ""
    echo "📦 Built plugins:"
    echo "   AU: $(find Builds_Intel -name "*.component" -type d)"
    echo "   VST3: $(find Builds_Intel -name "*.vst3" -type d)"
    echo ""
    echo "🎯 To install Intel plugins manually:"
    echo "   cp -r \"$(find Builds_Intel -name "*.component" -type d)\" ~/Library/Audio/Plug-Ins/Components/"
    echo "   cp -r \"$(find Builds_Intel -name "*.vst3" -type d)\" ~/Library/Audio/Plug-Ins/VST3/"
else
    echo "❌ Intel build failed!"
    exit 1
fi 