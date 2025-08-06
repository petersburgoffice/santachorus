cmake_minimum_required(VERSION 3.15)

project(SantaChorus VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Читаем версию из файла (версия управляется через increment_version.sh)
file(STRINGS ${CMAKE_SOURCE_DIR}/version.txt VERSION_STRING)
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_BUILD)

message(STATUS "Version configured: ${VERSION_STRING}")

# Генерируем Version.h из шаблона
configure_file(
    ${CMAKE_SOURCE_DIR}/Source/Version.h.in
    ${CMAKE_SOURCE_DIR}/Source/Version.h
    @ONLY
)

# Add JUCE from common location
add_subdirectory(../JUCE ${CMAKE_BINARY_DIR}/JUCE)

# Add Common library
add_subdirectory(../Common ${CMAKE_BINARY_DIR}/Common)

# Create the plugin target
juce_add_plugin(SantaChorus
    PRODUCT_NAME "Santa Chorus"
        COMPANY_NAME "SonicMakers"  
    BUNDLE_ID "com.sonicmakers.santachorus"
    PLUGIN_MANUFACTURER_CODE "SonM"
    PLUGIN_CODE "Satu"
    FORMATS AU VST3 Standalone
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
    Common
)

# Compiler definitions
target_compile_definitions(SantaChorus PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Автоматический деплой AU и VST3 версий после сборки
if(APPLE)
    # Пути к папкам плагинов
    set(AU_INSTALL_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/Components")
    set(VST3_INSTALL_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3")
    
    # Добавляем команду копирования AU после сборки
    add_custom_command(TARGET SantaChorus_AU POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${AU_INSTALL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "$<TARGET_BUNDLE_DIR:SantaChorus_AU>" 
            "${AU_INSTALL_DIR}/SantaChorus.component"
        COMMENT "Installing AU plugin to ${AU_INSTALL_DIR}"
    )
    
    # Добавляем команду копирования VST3 после сборки
    add_custom_command(TARGET SantaChorus_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VST3_INSTALL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "$<TARGET_BUNDLE_DIR:SantaChorus_VST3>" 
            "${VST3_INSTALL_DIR}/SantaChorus.vst3"
        COMMENT "Installing VST3 plugin to ${VST3_INSTALL_DIR}"
    )
    
    message(STATUS "AU plugin will be automatically installed to: ${AU_INSTALL_DIR}")
    message(STATUS "VST3 plugin will be automatically installed to: ${VST3_INSTALL_DIR}")
endif() 