cmake_minimum_required(VERSION 3.15)

project(SantaChorus VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Add JUCE to the project
add_subdirectory(JUCE)

# Create the plugin target
juce_add_plugin(SantaChorus
    PRODUCT_NAME "Santa Chorus"
        COMPANY_NAME "SonicMakers"  
    BUNDLE_ID "com.sonicmakers.santachorus"
    PLUGIN_MANUFACTURER_CODE "SonM"
    PLUGIN_CODE "Satu"
    FORMATS VST3 AU Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3"
    AU_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
    PLIST_TO_MERGE "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

# Add source files
target_sources(SantaChorus PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SaturatorEngine.cpp
)

# Add header files
target_sources(SantaChorus PRIVATE
    Source/PluginProcessor.h
    Source/PluginEditor.h
    Source/SaturatorEngine.h
)

# Add binary resources
juce_add_binary_data(SantaChorusData SOURCES
    Resources/full_bg.png
    Resources/santa_chorus_logo.png
)

# Link libraries
target_link_libraries(SantaChorus PRIVATE
    SantaChorusData
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_audio_basics
)

# Compiler definitions
target_compile_definitions(SaturVST PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
) 