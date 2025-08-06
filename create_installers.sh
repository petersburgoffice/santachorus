#!/bin/bash

# Saturos Pro Installer Creator
# Creates installers for both macOS and Windows

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${BLUE}🎵 Saturos Pro Universal Binary Installer Creator${NC}"
echo -e "${PURPLE}===============================================${NC}"

# Get version from version.txt
if [ -f "version.txt" ]; then
    VERSION=$(cat version.txt)
    echo -e "${GREEN}📦 Version: ${VERSION}${NC}"
else
    echo -e "${RED}❌ Error: version.txt not found${NC}"
    exit 1
fi

# Check if Universal Binary plugins are built
echo -e "${YELLOW}🔍 Checking if Universal Binary plugins are built...${NC}"

AU_PATH="Builds_Universal/SaturVST_artefacts/Release/AU/Saturos Pro.component"
VST3_PATH="Builds_Universal/SaturVST_artefacts/Release/VST3/Saturos Pro.vst3"

if [ ! -d "$AU_PATH" ] || [ ! -d "$VST3_PATH" ]; then
    echo -e "${YELLOW}⚠️  Universal Binary plugins not found. Building first...${NC}"
    ./build_universal.sh
    echo -e "${GREEN}✅ Universal Binary plugins built successfully${NC}"
fi

# Create Universal Binary macOS installer
echo -e "${BLUE}🍎 Creating Universal Binary macOS Installer...${NC}"
chmod +x Installers/macOS/create_universal_installer.sh
./Installers/macOS/create_universal_installer.sh

# Check if macOS installer was created
MACOS_INSTALLER="Installers/macOS/Santa_Chorus_${VERSION}_Universal_macOS_Installer.pkg"
if [ -f "$MACOS_INSTALLER" ]; then
    echo -e "${GREEN}✅ macOS installer created: ${MACOS_INSTALLER}${NC}"
    MACOS_SIZE=$(du -h "$MACOS_INSTALLER" | cut -f1)
    echo -e "${BLUE}📏 Size: ${MACOS_SIZE}${NC}"
else
    echo -e "${RED}❌ Failed to create macOS installer${NC}"
fi

echo ""
echo -e "${PURPLE}======================================${NC}"

# Windows installer information
echo -e "${BLUE}🪟 Windows Installer Information${NC}"
echo -e "${YELLOW}📝 To create Windows installer:${NC}"
echo -e "   1. Copy the project to a Windows machine"
echo -e "   2. Install NSIS from: https://nsis.sourceforge.io/Download"
echo -e "   3. Build the plugin on Windows using CMake"
echo -e "   4. Run: ${GREEN}Installers\\Windows\\create_installer.bat${NC}"
echo ""
echo -e "${YELLOW}📋 Windows installer will create:${NC}"
echo -e "   • ${GREEN}Santa_Chorus_${VERSION}_Windows_Installer.exe${NC}"
echo -e "   • Installs VST3 to: ${BLUE}C:\\Program Files\\Common Files\\VST3\\${NC}"
echo -e "   • Creates uninstaller and registry entries"

echo ""
echo -e "${PURPLE}======================================${NC}"
echo -e "${GREEN}🎉 Installer creation process completed!${NC}"

# Summary
echo ""
echo -e "${BLUE}📦 Created Installers:${NC}"
if [ -f "$MACOS_INSTALLER" ]; then
    echo -e "   ✅ macOS: ${GREEN}${MACOS_INSTALLER}${NC} (${MACOS_SIZE})"
else
    echo -e "   ❌ macOS: Failed"
fi
echo -e "   📋 Windows: See instructions above"

echo ""
echo -e "${YELLOW}💡 Important Notes:${NC}"
echo -e "   • Test installers before distribution"
echo -e "   • macOS Universal Binary works on Intel + Apple Silicon"
echo -e "   • macOS installer requires admin privileges"
echo -e "   • Windows installer requires NSIS and admin privileges"
echo -e "   • Both installers include uninstall functionality"

echo ""
echo -e "${BLUE}🔥 Ready to distribute Saturos Pro!${NC}" 