#include "PluginProcessor.h"
#include "PluginEditor.h"

SaturVSTProcessor::SaturVSTProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    valueTreeState(*this, nullptr, "Parameters", 
        juce::AudioProcessorValueTreeState::ParameterLayout{
            std::make_unique<juce::AudioParameterFloat>(
                "chorus", "Chorus", 
                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
            std::make_unique<juce::AudioParameterFloat>(
                "mix", "Dry/Wet", 
                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f)
        })
{
    // Initialize parameters: Chorus and Dry/Wet mix
    chorusParameter = valueTreeState.getRawParameterValue("chorus");
    mixParameter = valueTreeState.getRawParameterValue("mix");
    
    // Safety check
    if (!chorusParameter || !mixParameter)
    {
        // Log error - parameters not found!
        juce::Logger::writeToLog("ERROR: Failed to initialize parameters in SantaChorus!");
    }
}

SaturVSTProcessor::~SaturVSTProcessor()
{
}

const juce::String SaturVSTProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SaturVSTProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SaturVSTProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SaturVSTProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SaturVSTProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SaturVSTProcessor::getNumPrograms()
{
    return 1;
}

int SaturVSTProcessor::getCurrentProgram()
{
    return 0;
}

void SaturVSTProcessor::setCurrentProgram (int index)
{
}

const juce::String SaturVSTProcessor::getProgramName (int index)
{
    return {};
}

void SaturVSTProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SaturVSTProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    saturatorEngine.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void SaturVSTProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SaturVSTProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SaturVSTProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Safety check for parameters before accessing them
    if (chorusParameter && mixParameter)
    {
        try
        {
            // Update parameters: Chorus and Dry/Wet mix
            saturatorEngine.setChorus(chorusParameter->load());
            saturatorEngine.setMix(mixParameter->load());

            // Process audio
            saturatorEngine.processBlock(buffer);
        }
        catch (const std::exception& e)
        {
            // Log error but don't crash - just pass audio through
            juce::Logger::writeToLog("ERROR in processBlock: " + juce::String(e.what()));
            // Buffer already cleared above, so we just pass silence/input through
        }
    }
    else
    {
        // If parameters are not initialized, just log and pass audio through
        static bool errorLogged = false;
        if (!errorLogged)
        {
            juce::Logger::writeToLog("ERROR: Parameters not initialized in processBlock!");
            errorLogged = true; // Only log once to avoid spam
        }
        // Audio will pass through unchanged
    }
}

bool SaturVSTProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SaturVSTProcessor::createEditor()
{
    return new SaturVSTEditor (*this);
}

void SaturVSTProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SaturVSTProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaturVSTProcessor();
} 