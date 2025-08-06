#include "SaturatorEngine.h"
#include <cmath>

SaturatorEngine::SaturatorEngine()
{
}

SaturatorEngine::~SaturatorEngine()
{
}

void SaturatorEngine::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;
    currentNumChannels = numChannels;

    // Initialize smoothed values in processing order: Chorus → Drive → Mix → Output
    smoothedChorus.reset(sampleRate, 0.05); // 50ms ramp time
    smoothedDrive.reset(sampleRate, 0.05);
    smoothedMix.reset(sampleRate, 0.05);
    smoothedOutputGain.reset(sampleRate, 0.05);

    smoothedChorus.setCurrentAndTargetValue(chorus.load());
    smoothedDrive.setCurrentAndTargetValue(drive.load());
    smoothedMix.setCurrentAndTargetValue(mix.load());
    smoothedOutputGain.setCurrentAndTargetValue(outputGain.load());
    
    // Initialize advanced stereo chorus system
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    
    // Setup per-channel chorus processing
    chorusChannels.resize(numChannels);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        chorusChannels[ch].delayBuffer.resize(maxDelayInSamples, 0.0f);
        chorusChannels[ch].writeIndex = 0;
        chorusChannels[ch].prevSample = 0.0f;
        chorusChannels[ch].dcBlocker_x1 = 0.0f;
        chorusChannels[ch].dcBlocker_y1 = 0.0f;
        chorusChannels[ch].lpf_state = 0.0f;
    }
    
    // Setup new high-quality stereo LFOs
    lfoLeft.prepare(spec);
    lfoRight.prepare(spec);
    
    // Initialize stereo LFOs with phase offset for width
    lfoLeft.setFrequency(lfoRateHz);
    lfoLeft.initialise([](float x) { return std::sin(x); });
    
    // Right channel with 90° phase offset for stereo width
    lfoRight.setFrequency(lfoRateHz);
    lfoRight.initialise([](float x) { return std::sin(x + juce::MathConstants<float>::halfPi); });
    
    // Initialize smoothed delay for each channel to prevent artifacts
    for (auto& ch : chorusChannels)
    {
        ch.smoothedDelay.setCurrentAndTargetValue(minDelayMs);
        ch.smoothedDelay.reset(currentSampleRate, 0.02); // 20ms smoothing time
    }
}

void SaturatorEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Safety checks
    if (numSamples <= 0 || numChannels <= 0)
        return;

    try
    {
        // Update smoothed parameters
        smoothedChorus.setTargetValue(chorus.load());
        smoothedDrive.setTargetValue(drive.load());
        smoothedMix.setTargetValue(mix.load());
        smoothedOutputGain.setTargetValue(outputGain.load());

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                const float inputSample = channelData[sample];

                // Skip processing if input is not finite
                if (!std::isfinite(inputSample))
                {
                    channelData[sample] = 0.0f;
                    continue;
                }

                const float currentChorus = smoothedChorus.getNextValue();
                const float currentDrive = smoothedDrive.getNextValue();
                const float currentMix = smoothedMix.getNextValue();
                const float currentOutputGain = smoothedOutputGain.getNextValue();

                // STEP 1: Apply high-quality chorus effect
                float chorusProcessedSample = inputSample;
                
                if (currentChorus > 0.001f) // Only apply chorus if there's a meaningful amount
                {
                    chorusProcessedSample = processHighQualityChorus(inputSample, channel, currentChorus);
                }

                // STEP 2: Apply drive/saturation after chorus
                const float saturatedSample = saturate(chorusProcessedSample, currentDrive);

                // STEP 3: Mix wet and dry signals  
                const float mixedSample = inputSample * (1.0f - currentMix) + saturatedSample * currentMix;

                // STEP 4: Apply output gain
                float outputSample = mixedSample * currentOutputGain;
                
                // Clamp to reasonable range and ensure finite
                if (!std::isfinite(outputSample))
                    outputSample = 0.0f;
                else
                    outputSample = juce::jlimit(-10.0f, 10.0f, outputSample);

                channelData[sample] = outputSample;
            }
        }
    }
    catch (const std::exception& e)
    {
        // Log error and clear buffer to avoid further issues
        juce::Logger::writeToLog("ERROR in SaturatorEngine::processBlock: " + juce::String(e.what()));
        buffer.clear();
    }
}

// High-quality chorus processing (based on professional implementations)
float SaturatorEngine::processHighQualityChorus(float inputSample, int channel, float chorusAmount)
{
    auto& ch = chorusChannels[channel];
    
    // Apply DC blocking first for clean sound
    float cleanSample = dcBlocker(inputSample, channel);
    
    // Generate stereo LFO modulation with phase offset for width
    float lfoValue;
    if (channel == 0)
    {
        lfoValue = lfoLeft.processSample(0.0f);
    }
    else
    {
        lfoValue = lfoRight.processSample(0.0f);
    }
    
    // Calculate modulated delay time with professional scaling
    const float delayRange = (maxDelayMs - minDelayMs) * 0.5f;
    const float centerDelay = minDelayMs + delayRange;
    const float modulationDepth = delayRange * chorusAmount * lfoDepthScale;
    const float targetDelayMs = centerDelay + (lfoValue * modulationDepth);
    
    // Use smoothed delay to prevent artifacts
    ch.smoothedDelay.setTargetValue(targetDelayMs);
    const float smoothedDelayMs = ch.smoothedDelay.getNextValue();
    
    // Convert to samples with proper bounds checking
    const float delaySamples = juce::jlimit(1.0f, static_cast<float>(maxDelayInSamples - 1),
                                           (smoothedDelayMs / 1000.0f) * static_cast<float>(currentSampleRate));
    
    // Apply high-quality linear interpolation
    const float delayedSample = linearInterpolation(delaySamples, channel, cleanSample);
    
    // Mix dry and wet signals with proper gain compensation
    const float dryGain = 1.0f - (chorusAmount * 0.3f); // Slight dry reduction for depth
    const float wetGain = chorusAmount * 0.6f; // Wet gain for effect strength
    
    return inputSample * dryGain + delayedSample * wetGain;
}

// High-quality linear interpolation (more stable than all-pass for modulated delays)
float SaturatorEngine::linearInterpolation(float delayInSamples, int channel, float inputSample)
{
    auto& ch = chorusChannels[channel];
    
    // Write input sample to delay buffer
    ch.delayBuffer[ch.writeIndex] = inputSample;
    
    // Calculate integer and fractional parts of delay
    const int integerDelay = static_cast<int>(std::floor(delayInSamples));
    const float fractionalDelay = delayInSamples - integerDelay;
    
    // Calculate read indices with proper bounds checking
    const int bufferSize = static_cast<int>(ch.delayBuffer.size());
    int readIndex1 = (ch.writeIndex - integerDelay + bufferSize) % bufferSize;
    int readIndex2 = (ch.writeIndex - integerDelay - 1 + bufferSize) % bufferSize;
    
    // Get samples for interpolation
    const float sample1 = ch.delayBuffer[readIndex1];
    const float sample2 = ch.delayBuffer[readIndex2];
    
    // Linear interpolation with anti-aliasing low-pass filter
    float interpolated = sample1 * (1.0f - fractionalDelay) + sample2 * fractionalDelay;
    
    // Simple one-pole low-pass filter for anti-aliasing (cutoff at ~8kHz)
    const float lpfCutoff = 0.7f;
    ch.lpf_state = ch.lpf_state + lpfCutoff * (interpolated - ch.lpf_state);
    
    // Update write index
    ch.writeIndex = (ch.writeIndex + 1) % bufferSize;
    
    // Ensure output is finite
    if (!std::isfinite(ch.lpf_state))
        return inputSample;
    
    return ch.lpf_state;
}

// DC blocking filter for clean sound
float SaturatorEngine::dcBlocker(float inputSample, int channel)
{
    auto& ch = chorusChannels[channel];
    
    // High-pass filter: y[n] = x[n] - x[n-1] + 0.995 * y[n-1]
    const float output = inputSample - ch.dcBlocker_x1 + 0.995f * ch.dcBlocker_y1;
    
    ch.dcBlocker_x1 = inputSample;
    ch.dcBlocker_y1 = output;
    
    // Ensure output is finite
    if (!std::isfinite(output))
        return inputSample;
    
    return output;
}

// Parameter setters in new order: Chorus → Drive → Mix → Output
void SaturatorEngine::setChorus(float newChorus)
{
    chorus.store(juce::jlimit(0.0f, 1.0f, newChorus));
}

void SaturatorEngine::setDrive(float newDrive)
{
    drive.store(juce::jlimit(1.0f, 10.0f, newDrive));
}

void SaturatorEngine::setMix(float newMix)
{
    mix.store(juce::jlimit(0.0f, 1.0f, newMix));
}

void SaturatorEngine::setOutputGain(float newGain)
{
    outputGain.store(juce::jlimit(0.1f, 2.0f, newGain));
}

float SaturatorEngine::saturate(float input, float drive)
{
    // Use tanh saturation as primary algorithm
    return tanhSaturation(input, drive);
}

float SaturatorEngine::tanhSaturation(float input, float drive)
{
    // Apply drive and tanh saturation
    const float drivenInput = input * drive;
    return std::tanh(drivenInput) / drive; // Compensate for drive to maintain roughly same output level
}

float SaturatorEngine::softClipping(float input, float drive)
{
    // Soft clipping algorithm
    const float drivenInput = input * drive;
    
    if (std::abs(drivenInput) < 0.33f)
        return drivenInput;
    else if (std::abs(drivenInput) < 0.66f)
        return juce::jmax(-0.66f, juce::jmin(0.66f, drivenInput)) * (2.0f - std::abs(drivenInput) * 3.0f);
    else
        return drivenInput > 0.0f ? 0.66f : -0.66f;
    
    return drivenInput / drive; // Compensate for drive
} 