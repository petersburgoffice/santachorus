#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class SaturatorEngine
{
public:
    SaturatorEngine();
    ~SaturatorEngine();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Parameter setters: Chorus and Dry/Wet mix
    void setChorus(float newChorus);
    void setMix(float newMix);

private:

    // High-quality chorus processing (based on professional implementations)
    struct ChorusChannel
    {
        // Delay line buffer (larger for better quality)
        std::vector<float> delayBuffer;
        int writeIndex = 0;
        
        // Linear interpolation state (more stable than all-pass for modulated delays)
        float prevSample = 0.0f;
        
        // Smoothed delay parameter to avoid artifacts
        juce::SmoothedValue<float> smoothedDelay;
        
        // DC blocker for clean sound
        float dcBlocker_x1 = 0.0f;
        float dcBlocker_y1 = 0.0f;
        
        // Low-pass filter for anti-aliasing
        float lpf_state = 0.0f;
    };
    
    // Per-channel chorus processing
    std::vector<ChorusChannel> chorusChannels;
    
    // Stereo LFOs with phase offset for width
    juce::dsp::Oscillator<float> lfoLeft;   // Left channel LFO
    juce::dsp::Oscillator<float> lfoRight;  // Right channel LFO (90° phase offset)
    
    // High-quality interpolation
    float linearInterpolation(float delayInSamples, int channel, float inputSample);
    
    // Professional chorus processing
    float processHighQualityChorus(float inputSample, int channel, float chorusAmount);
    
    // DC blocking filter
    float dcBlocker(float inputSample, int channel);

    // Parameters
    std::atomic<float> chorus{ 0.5f };
    std::atomic<float> mix{ 0.5f };

    // Processing variables
    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    int currentNumChannels = 2;

    // Smoothed parameters to avoid zipper noise
    juce::SmoothedValue<float> smoothedChorus;
    juce::SmoothedValue<float> smoothedMix;
    
    // Professional chorus parameters (based on high-quality implementations)
    static constexpr int maxDelayInSamples = 1764;  // 40ms at 44.1kHz
    static constexpr float minDelayMs = 2.5f;       // Minimum delay: 2.5ms (prevents flanging)
    static constexpr float maxDelayMs = 15.0f;      // Maximum delay: 15ms (classic chorus range)
    static constexpr float lfoRateHz = 0.5f;        // 0.5 Hz (classic chorus rate)
    static constexpr float lfoDepthScale = 0.8f;    // Maximum LFO depth scaling
}; 