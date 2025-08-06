# 🎅 Santa Chorus VST Plugin

**Festive Holiday-Themed Audio Chorus Plugin**

A professional VST3/AU audio plugin with magical holiday design and beautiful chorus effects.

![Santa Chorus Interface](https://github.com/petersburgoffice/santachorus/raw/main/Resources/full_bg.png)

## ✨ Features

### 🎛️ Audio Controls
- **CHORUS** - Add depth and dimension to your sound
- **DRIVE** - Powerful saturation and distortion engine  
- **MIX** - Blend between dry and processed signal
- **OUTPUT** - Final level control

### 🎨 Design
- **Magical holiday-themed background** with SANTA CHORUS branding
- **Custom rotary knobs** with colored rings matching the interface
- **Precise positioning** - controls perfectly aligned with background design
- **Professional metal band aesthetic**
- **772x445 pixel interface** optimized for modern displays

### 🔧 Technical Specs
- **JUCE Framework** - Cross-platform audio plugin development
- **VST3 & AU formats** - Compatible with all major DAWs
- **Standalone application** - Use without a DAW
- **macOS & Windows support** (Linux compatible)
- **Real-time audio processing** with low latency

## 🚀 Quick Start

### Prerequisites
- **CMake** 3.15 or higher
- **Xcode** (macOS) or **Visual Studio** (Windows)
- **Git** with SSH access

### Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/petersburgoffice/saturos.git
   cd saturos
   ```

2. **Quick build and run:**
   ```bash
   ./dev run
   ```

3. **Available commands:**
   ```bash
   ./dev build     # Full build
   ./dev quick     # Fast incremental build
   ./dev run       # Build + launch standalone
   ./dev clean     # Clean build directory
   ./dev rebuild   # Full rebuild
   ./dev help      # Show all options
   ```

### Installation
After building, plugins are automatically installed to:
- **VST3:** `~/Library/Audio/Plug-Ins/VST3/`
- **AU:** `~/Library/Audio/Plug-Ins/Components/`

## 🎵 Usage

1. **Load** SATUROS Pro in your DAW as a VST3 or AU plugin
2. **Add saturation** with the DRIVE knob
3. **Shape the sound** with CHORUS for added dimension
4. **Blend** the effect using the MIX control
5. **Set levels** with the OUTPUT knob

Perfect for:
- 🎸 **Guitar processing** - Add warmth and character
- 🥁 **Drum enhancement** - Punch and presence  
- 🎤 **Vocal saturation** - Analog warmth
- 🎹 **Instrument coloring** - Vintage vibe
- 🎚️ **Mix bus processing** - Glue and cohesion

## 🛠️ Development

### Project Structure
```
santachorus/
├── Source/              # C++ source files
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.cpp
│   └── SaturatorEngine.cpp
├── Resources/           # Images and assets
│   ├── full_bg.png     # Main background
│   └── santa_chorus_logo.png
├── JUCE/               # JUCE framework (submodule)
├── build_dev.sh        # Development build script
├── dev                 # Quick build alias
└── BUILD_COMMANDS.md   # Build documentation
```

### Build System
- **CMake** for cross-platform builds
- **JUCE** framework integration
- **Automatic plugin installation**
- **Development-friendly scripts**

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

**Proprietary Software** - All rights reserved.

This software is proprietary and confidential. Unauthorized copying, distribution, or use is strictly prohibited.

## 🔗 Links

- **Repository:** https://github.com/petersburgoffice/saturos
- **Issues:** https://github.com/petersburgoffice/saturos/issues
- **Releases:** https://github.com/petersburgoffice/saturos/releases

## 🎉 Credits

**Developed by:** Alexander Zverev (@azverev)  
**Company:** SonicMakers  
**Design:** Epic fire-themed interface with professional metal aesthetic

---

*Transform your audio with the power of fire* 🔥 