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

    // Parameter setters in new processing order: Chorus → Drive → Mix → Output
    void setChorus(float newChorus);
    void setDrive(float newDrive);
    void setMix(float newMix);
    void setOutputGain(float newGain);

private:
    // Saturation algorithms
    float saturate(float input, float drive);
    float tanhSaturation(float input, float drive);
    float softClipping(float input, float drive);

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

    // Parameters in processing order
    std::atomic<float> chorus{ 0.0f };
    std::atomic<float> drive{ 2.0f };
    std::atomic<float> mix{ 0.5f };
    std::atomic<float> outputGain{ 1.0f };

    // Processing variables
    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    int currentNumChannels = 2;

    // Smoothed parameters to avoid zipper noise (in processing order)
    juce::SmoothedValue<float> smoothedChorus;
    juce::SmoothedValue<float> smoothedDrive;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float> smoothedOutputGain;
    
    // Professional chorus parameters (based on high-quality implementations)
    static constexpr int maxDelayInSamples = 1764;  // 40ms at 44.1kHz
    static constexpr float minDelayMs = 2.5f;       // Minimum delay: 2.5ms (prevents flanging)
    static constexpr float maxDelayMs = 15.0f;      // Maximum delay: 15ms (classic chorus range)
    static constexpr float lfoRateHz = 0.5f;        // 0.5 Hz (classic chorus rate)
    static constexpr float lfoDepthScale = 0.8f;    // Maximum LFO depth scaling
}; 