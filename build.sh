#!/bin/bash

# SaturVST Build Script

set -e  # Exit on any error

echo "🎵 Building SaturVST Plugin..."

# Check if JUCE exists
if [ ! -d "JUCE" ]; then
    echo "📦 JUCE not found. Cloning JUCE framework..."
    git clone https://github.com/juce-framework/JUCE.git
    echo "✅ JUCE cloned successfully"
fi

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
    echo "📁 Created build directory"
fi

cd build

# Configure with CMake
echo "⚙️  Configuring with CMake..."
cmake ..

# Build the plugin
echo "🔨 Building plugin..."
cmake --build . --config Release

echo "🎉 Build completed successfully!"
echo ""
echo "Plugin installed to:"
echo "  VST3: ~/Library/Audio/Plug-Ins/VST3/"
echo "  AU:   ~/Library/Audio/Plug-Ins/Components/"
echo ""
echo "You can now load SaturVST in your DAW!" 