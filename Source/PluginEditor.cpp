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
    /*
    // Knob gradient from light to dark (simulating light from top-left)
    juce::ColourGradient knobGradient(juce::Colour(0xff4a4a4a), knobBounds.getTopLeft(),
                                     juce::Colour(0xff2a2a2a), knobBounds.getBottomRight(), false);
    g.setGradientFill(knobGradient);
    g.fillEllipse(knobBounds);

    // Knob border
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawEllipse(knobBounds, 1.0f);
*/
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
    mixLookAndFeel = std::make_unique<CustomRotarySliderLookAndFeel>(juce::Colour(0xff2196F3)); // Blue

    // Setup Chorus slider
    chorusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusSlider.setRange(0.0, 1.0, 0.01);
    chorusSlider.setValue(0.0);
    chorusSlider.setLookAndFeel(chorusLookAndFeel.get());
    addAndMakeVisible(chorusSlider);

    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(juce::FontOptions().withHeight(14.0f)));
    chorusLabel.setJustificationType(juce::Justification::centred);
    chorusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00BCD4));
    addAndMakeVisible(chorusLabel);



    // Setup Mix slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setValue(0.5);
    mixSlider.setLookAndFeel(mixLookAndFeel.get());
    addAndMakeVisible(mixSlider);

    mixLabel.setText("DRY/WET", juce::dontSendNotification);
    mixLabel.setFont(juce::Font(juce::FontOptions().withHeight(14.0f)));
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2196F3));
    addAndMakeVisible(mixLabel);



    // Setup Version label (bottom right corner)
    versionLabel.setText("v" + juce::String(PLUGIN_VERSION_STRING), juce::dontSendNotification);
    versionLabel.setFont(juce::Font(juce::FontOptions().withHeight(10.0f)));
    versionLabel.setJustificationType(juce::Justification::centredRight);
    versionLabel.setColour(juce::Label::textColourId, juce::Colour(0x80FFFFFF)); // Semi-transparent white
    addAndMakeVisible(versionLabel);

    // Create parameter attachments
    chorusAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "chorus", chorusSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "mix", mixSlider);

    // Устанавливаем размер окна точно под размер background изображения (769 × 398)
    setSize(769, 398);
}

SaturVSTEditor::~SaturVSTEditor()
{
    chorusSlider.setLookAndFeel(nullptr);
    mixSlider.setLookAndFeel(nullptr);
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
    // Позиционирование двух ручек на background изображении (769x398)
    const int knobSize = 240; // Уменьшаем размер ручек пропорционально
    
    const int knobY = 29;
    // Размещаем две ручки симметрично (пересчитано для нового размера)
    // Chorus knob (левая)
    chorusSlider.setBounds(25, knobY, knobSize, knobSize);
    
    // Dry/Wet knob (правая)
    mixSlider.setBounds(509, knobY, knobSize, knobSize);
    
    // Скрываем все надписи (они видны на background изображении)
    chorusLabel.setBounds(0, 0, 0, 0);
    mixLabel.setBounds(0, 0, 0, 0);
    
    // Position version label in bottom right corner
    versionLabel.setBounds(getWidth() - 80, getHeight() - 20, 75, 15);
} 