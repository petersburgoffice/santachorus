# 📦 Saturos Pro Installers

This directory contains scripts and resources for creating professional installers for **Saturos Pro** on both macOS and Windows platforms.

## 🚀 Quick Start

### Create All Installers
```bash
# From project root directory
./create_installers.sh
```

This script will:
- ✅ Check if plugins are built (builds them if needed)
- 🍎 Create macOS installer automatically
- 📋 Provide instructions for Windows installer

## 🍎 macOS Installer

### Requirements
- macOS with Xcode Command Line Tools
- Built AU and VST3 plugins

### Usage
```bash
cd Installers/macOS
./create_installer.sh
```

### Output
- **File:** `Santa_Chorus_[VERSION]_macOS_Installer.pkg`
- **Size:** ~50-60 MB
- **Installs:** AU and VST3 plugins to system directories

### Features
- ✅ Professional installer with welcome, license, and completion screens
- ✅ Installs to standard plugin directories
- ✅ Automatic code signing and permissions
- ✅ Uninstaller support
- ✅ Compatible with Intel and Apple Silicon Macs

### Installation Locations
- **AU:** `/Library/Audio/Plug-Ins/Components/Saturos Pro.component`
- **VST3:** `/Library/Audio/Plug-Ins/VST3/Saturos Pro.vst3`

## 🪟 Windows Installer

### Requirements
- Windows 10/11
- [NSIS (Nullsoft Scriptable Install System)](https://nsis.sourceforge.io/Download)
- Built VST3 plugin (Windows build)

### Setup NSIS
1. Download NSIS from: https://nsis.sourceforge.io/Download
2. Install NSIS with default settings
3. Make sure "Add NSIS to PATH" is checked during installation

### Usage
```batch
cd Installers\Windows
create_installer.bat
```

### Output
- **File:** `Santa_Chorus_[VERSION]_Windows_Installer.exe`
- **Size:** ~30-40 MB
- **Installs:** VST3 plugin to system directory

### Features
- ✅ Modern installer with multiple pages
- ✅ License agreement screen
- ✅ Component selection (plugin, shortcuts)
- ✅ Registry entries for proper uninstallation
- ✅ Start Menu and Desktop shortcuts (optional)
- ✅ Automatic upgrade handling

### Installation Location
- **VST3:** `C:\Program Files\Common Files\VST3\Saturos Pro.vst3`

## 📁 Directory Structure

```
Installers/
├── README.md                          # This file
├── macOS/
│   ├── create_installer.sh           # macOS installer script
│   └── temp_installer/               # Temporary build files
└── Windows/
    ├── create_installer.bat          # Windows installer script
    ├── santa_chorus_installer.nsi     # NSIS script
    └── license.txt                   # License for installer
```

## 🔧 Customization

### Updating Version
Version is automatically read from `version.txt` in the project root. The installer scripts will use this version for:
- Installer filenames
- Package metadata
- Registry entries

### Modifying License
- **macOS:** License is embedded in the installer script
- **Windows:** Edit `Installers/Windows/license.txt`

### Adding Icons
- **macOS:** Icons are embedded in the plugin bundles
- **Windows:** Add `icon.ico` to `Resources/` directory and update NSIS script

## 🧪 Testing

### macOS Testing
```bash
# Install the package
sudo installer -pkg "Santa_Chorus_[VERSION]_macOS_Installer.pkg" -target /

# Verify installation
ls -la /Library/Audio/Plug-Ins/Components/Saturos\ Pro.component
ls -la /Library/Audio/Plug-Ins/VST3/Saturos\ Pro.vst3

# Test in DAW
# Open Logic Pro, Ableton Live, or your preferred DAW
# Look for "Saturos Pro" in audio effects
```

### Windows Testing
```batch
# Run installer as administrator
Santa_Chorus_[VERSION]_Windows_Installer.exe

# Verify installation
dir "C:\Program Files\Common Files\VST3\Saturos Pro.vst3"

# Check uninstaller in Control Panel
# Programs and Features -> Saturos Pro
```

## 🚨 Troubleshooting

### macOS Issues

**"Package is damaged"**
- Ensure plugins are properly built and signed
- Check file permissions on installer script

**"Permission denied"**
- Run installer with `sudo` if needed
- Check that target directories are writable

### Windows Issues

**"NSIS not found"**
- Install NSIS and add to PATH
- Restart command prompt after installation

**"Plugin not found"**
- Build the Windows version of the plugin first
- Ensure VST3 file exists in build directory

**"Access denied"**
- Run as Administrator
- Check antivirus software isn't blocking

## 📋 Distribution Checklist

Before distributing installers:

- [ ] Test installation on clean system
- [ ] Verify plugin loads in multiple DAWs
- [ ] Test uninstallation process
- [ ] Check installer file sizes are reasonable
- [ ] Verify version numbers are correct
- [ ] Test on different OS versions
- [ ] Scan for malware/viruses (especially Windows)

## 🔐 Security Notes

### Code Signing
- **macOS:** Uses ad-hoc signing (for development)
- **Windows:** No signing (for development)

For production distribution, consider:
- Apple Developer ID certificate (macOS)
- Code signing certificate (Windows)
- Notarization (macOS)

### Antivirus
Windows installers may trigger antivirus warnings. For production:
- Use signed executables
- Submit to antivirus vendors for whitelisting
- Use established installer frameworks

---

**🔥 Ready to distribute Saturos Pro to the world!** 