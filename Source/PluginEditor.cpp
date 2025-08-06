#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

void CustomRotarySliderLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                                     float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                                     juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(6.0f, radius * 0.4f);
    auto arcRadius = radius - lineW * 0.5f;

    // Draw the background arc (full circle)
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                               bounds.getCentreY(),
                               arcRadius,
                               arcRadius,
                               0.0f,
                               rotaryStartAngle,
                               rotaryEndAngle,
                               true);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Draw the value arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
                              bounds.getCentreY(),
                              arcRadius,
                              arcRadius,
                              0.0f,
                              rotaryStartAngle,
                              toAngle,
                              true);

        g.setColour(ringColour);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw the knob with consistent size
    auto knobRadius = radius - lineW * 1.8f;
    auto knobBounds = juce::Rectangle<float>(knobRadius * 2.0f, knobRadius * 2.0f).withCentre(bounds.getCentre());
    
    // Knob shadow
    auto shadowBounds = knobBounds.translated(1, 2);
    g.setColour(juce::Colour(0x30000000));
    g.fillEllipse(shadowBounds);
    
    // Knob gradient from light to dark (simulating light from top-left)
    juce::ColourGradient knobGradient(juce::Colour(0xff4a4a4a), knobBounds.getTopLeft(),
                                     juce::Colour(0xff2a2a2a), knobBounds.getBottomRight(), false);
    g.setGradientFill(knobGradient);
    g.fillEllipse(knobBounds);

    // Knob border
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawEllipse(knobBounds, 1.0f);

    // Draw the pointer with consistent style
    juce::Path pointer;
    auto pointerLength = knobRadius * 0.7f;
    auto pointerThickness = 2.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -knobRadius + pointerLength * 0.4f, pointerThickness, pointerLength);
    
    pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
    
    g.setColour(ringColour.brighter(0.4f));
    g.fillPath(pointer);
}

SaturVSTEditor::SaturVSTEditor (SaturVSTProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Load background image from binary data
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::full_bg_png, BinaryData::full_bg_pngSize);

    // Create custom look and feel objects with specific colors
    chorusLookAndFeel = std::make_unique<CustomRotarySliderLookAndFeel>(juce::Colour(0xff00BCD4)); // Cyan/Teal
    driveLookAndFeel = std::make_unique<CustomRotarySliderLookAndFeel>(juce::Colour(0xffFF9800)); // Orange
    mixLookAndFeel = std::make_unique<CustomRotarySliderLookAndFeel>(juce::Colour(0xff2196F3)); // Blue
    outputLookAndFeel = std::make_unique<CustomRotarySliderLookAndFeel>(juce::Colour(0xffE0E0E0)); // Light Gray

    // Setup Chorus slider
    chorusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusSlider.setRange(0.0, 1.0, 0.01);
    chorusSlider.setValue(0.0);
    chorusSlider.setLookAndFeel(chorusLookAndFeel.get());
    addAndMakeVisible(chorusSlider);

    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(14.0f, juce::Font::plain)); // Thinner font
    chorusLabel.setJustificationType(juce::Justification::centred);
    chorusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00BCD4));
    addAndMakeVisible(chorusLabel);

    // Setup Drive slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setRange(1.0, 10.0, 0.1);
    driveSlider.setValue(2.0);
    driveSlider.setLookAndFeel(driveLookAndFeel.get());
    addAndMakeVisible(driveSlider);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setFont(juce::Font(14.0f, juce::Font::plain)); // Thinner font
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setColour(juce::Label::textColourId, juce::Colour(0xffFF9800));
    addAndMakeVisible(driveLabel);

    // Setup Mix slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setValue(0.5);
    mixSlider.setLookAndFeel(mixLookAndFeel.get());
    addAndMakeVisible(mixSlider);

    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setFont(juce::Font(14.0f, juce::Font::plain)); // Thinner font
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2196F3));
    addAndMakeVisible(mixLabel);

    // Setup Output slider
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outputGainSlider.setRange(0.1, 2.0, 0.01);
    outputGainSlider.setValue(1.0);
    outputGainSlider.setLookAndFeel(outputLookAndFeel.get());
    addAndMakeVisible(outputGainSlider);

    outputGainLabel.setText("OUTPUT", juce::dontSendNotification);
    outputGainLabel.setFont(juce::Font(14.0f, juce::Font::plain)); // Thinner font
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.setColour(juce::Label::textColourId, juce::Colour(0xffE0E0E0));
    addAndMakeVisible(outputGainLabel);

    // Setup Version label (bottom right corner)
    versionLabel.setText("v" + juce::String(PLUGIN_VERSION_STRING), juce::dontSendNotification);
    versionLabel.setFont(juce::Font(10.0f, juce::Font::plain)); // Small font
    versionLabel.setJustificationType(juce::Justification::centredRight);
    versionLabel.setColour(juce::Label::textColourId, juce::Colour(0x80FFFFFF)); // Semi-transparent white
    addAndMakeVisible(versionLabel);

    // Create parameter attachments
    chorusAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "chorus", chorusSlider);
    
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "drive", driveSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "mix", mixSlider);
    
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "output", outputGainSlider);

    // Устанавливаем размер окна точно под размер background изображения
    setSize(772, 445);
}

SaturVSTEditor::~SaturVSTEditor()
{
    chorusSlider.setLookAndFeel(nullptr);
    driveSlider.setLookAndFeel(nullptr);
    mixSlider.setLookAndFeel(nullptr);
    outputGainSlider.setLookAndFeel(nullptr);
}

void SaturVSTEditor::paint (juce::Graphics& g)
{
    // Отображаем background изображение на весь размер окна
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat(), 
                   juce::RectanglePlacement::fillDestination);
    }
    else
    {
        // Fallback цвет если изображение не загрузилось
        g.setColour(juce::Colour(0xff2a2a2a));
        g.fillAll();
    }
}

void SaturVSTEditor::resized()
{
    // Точное позиционирование ручек на местах из background изображения
    const int knobSize = 92; // Размер ручек 49 пикселей
    
    // Координаты ручек основаны на позициях в background изображении 772x445
    // Chorus knob (левая верхняя)
    chorusSlider.setBounds(166, 272, knobSize, knobSize);
    
    // Drive knob (левая центральная) 
    driveSlider.setBounds(285, 272, knobSize, knobSize);
    
    // Mix knob (правая центральная)
    mixSlider.setBounds(402, 272, knobSize, knobSize);
    
    // Output knob (правая верхняя)
    outputGainSlider.setBounds(517, 272, knobSize, knobSize);
    
    // Скрываем все надписи (ставим их за пределы видимой области)
    chorusLabel.setBounds(0, 0, 0, 0);
    driveLabel.setBounds(0, 0, 0, 0);
    mixLabel.setBounds(0, 0, 0, 0);
    outputGainLabel.setBounds(0, 0, 0, 0);
    
    // Position version label in bottom right corner
    versionLabel.setBounds(getWidth() - 80, getHeight() - 20, 75, 15);
} 