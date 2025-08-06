#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Version.h"

class CustomRotarySliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomRotarySliderLookAndFeel(juce::Colour ringColour) : ringColour(ringColour) {}
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;

private:
    juce::Colour ringColour;
};

class SaturVSTEditor : public juce::AudioProcessorEditor
{
public:
    SaturVSTEditor (SaturVSTProcessor&);
    ~SaturVSTEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SaturVSTProcessor& audioProcessor;

    // Helper function for drawing signal flow arrows
    void drawArrow(juce::Graphics& g, int x1, int y, int x2, int yEnd);

    // UI Components: Chorus and Dry/Wet
    juce::Slider chorusSlider;
    juce::Label chorusLabel;
    
    juce::Slider mixSlider;
    juce::Label mixLabel;

    // Attachments for parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    // Custom look and feel objects
    std::unique_ptr<CustomRotarySliderLookAndFeel> chorusLookAndFeel;
    std::unique_ptr<CustomRotarySliderLookAndFeel> mixLookAndFeel;

    // Background image
    juce::Image backgroundImage;
    
    // Version label
    juce::Label versionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaturVSTEditor)
}; 