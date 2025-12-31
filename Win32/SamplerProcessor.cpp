/*
  ==============================================================================

    SamplerProcessor.cpp
    Win32 API Sampler Processor Implementation

  ==============================================================================
*/

#include "SamplerProcessor.h"
#include <algorithm>
#include <cmath>

SamplerProcessor::SamplerProcessor()
{
    // Initialize parameters with default values
    setParameter(PARAM_SEMITONE, 0.0f);
    setParameter(PARAM_CENT, 0.0f);
    setParameter(PARAM_WAVEFORM_SEMITONE, 0.0f);
    setParameter(PARAM_WAVEFORM_CENT, 0.0f);
    setParameter(PARAM_ATTACK_TIME, 0.01f);
    setParameter(PARAM_ATTACK_CURVE, 0.5f);
    setParameter(PARAM_RELEASE_TIME, 0.1f);
    setParameter(PARAM_RELEASE_CURVE, 0.5f);
    setParameter(PARAM_PLAYBACK_SPEED, 1.0f);
    setParameter(PARAM_PLAYBACK_MODE, 0.0f);
    setParameter(PARAM_LOFI_MODE, 0.0f);
    setParameter(PARAM_LOOP_ENABLED, 0.0f);
    setParameter(PARAM_LOOP_START, 0.0f);
    setParameter(PARAM_LOOP_END, 1.0f);
    setParameter(PARAM_MONO_OUTPUT, 0.0f);
    setParameter(PARAM_GAIN, 0.75f);
    setParameter(PARAM_EQ_ENABLED, 0.0f);
    setParameter(PARAM_REVERB_ENABLED, 0.0f);
    setParameter(PARAM_DISTORTION_ENABLED, 0.0f);
    setParameter(PARAM_CHORUS_ENABLED, 0.0f);

    // Initialize reverb buffer
    m_effectsState.reverbBuffer.resize(44100, 0.0f);
}

SamplerProcessor::~SamplerProcessor()
{
}

void SamplerProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    Win32AudioProcessor::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    
    // Initialize voices
    initializeVoices(8);
    
    // Resize effects buffers based on sample rate
    m_effectsState.reverbBuffer.resize(static_cast<size_t>(sampleRate), 0.0f);
}

void SamplerProcessor::releaseResources()
{
    Win32AudioProcessor::releaseResources();
}

void SamplerProcessor::initializeVoices(int numVoices)
{
    m_synth.clearVoices();
    
    for (int i = 0; i < numVoices; ++i)
    {
        SamplerVoice* voice = new SamplerVoice();
        m_synth.addVoice(voice);
    }
    
    m_synth.setSampleBuffer(&m_sampleBuffer, m_bufferSampleRate.load());
}

void SamplerProcessor::processBlock(AudioBuffer& buffer, MidiBuffer& midiMessages)
{
    // Clear output buffer
    buffer.clear();
    
    // Render synthesizer
    m_synth.renderNextBlock(buffer, midiMessages);
    
    // Apply effects
    processEffects(buffer);
    
    // Apply gain
    float gain = getParameter(PARAM_GAIN);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            channelData[i] *= gain;
        }
    }
    
    // Mono output if enabled
    if (getParameter(PARAM_MONO_OUTPUT) > 0.5f && buffer.getNumChannels() >= 2)
    {
        float* left = buffer.getWritePointer(0);
        float* right = buffer.getWritePointer(1);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float mono = (left[i] + right[i]) * 0.5f;
            left[i] = mono;
            right[i] = mono;
        }
    }
}

void SamplerProcessor::processEffects(AudioBuffer& buffer)
{
    if (getParameter(PARAM_EQ_ENABLED) > 0.5f)
        processEQ(buffer);
        
    if (getParameter(PARAM_DISTORTION_ENABLED) > 0.5f)
        processDistortion(buffer);
        
    if (getParameter(PARAM_CHORUS_ENABLED) > 0.5f)
        processChorus(buffer);
        
    if (getParameter(PARAM_REVERB_ENABLED) > 0.5f)
        processReverb(buffer);
}

void SamplerProcessor::processEQ(AudioBuffer& buffer)
{
    // Simple EQ implementation (placeholder)
    // A real implementation would use biquad filters
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        // Apply simple gain adjustments as a placeholder
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // This is a very simplified EQ - real implementation would use proper filters
            channelData[i] *= m_effectsState.midGain;
        }
    }
}

void SamplerProcessor::processReverb(AudioBuffer& buffer)
{
    // Simple reverb using delay lines (very basic implementation)
    const float reverbMix = 0.3f;
    const int reverbDelay = 10000; // samples
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = channelData[i];
            
            // Read from delay buffer
            float delayed = m_effectsState.reverbBuffer[m_effectsState.reverbPosition];
            
            // Write to delay buffer (with feedback)
            m_effectsState.reverbBuffer[m_effectsState.reverbPosition] = 
                input + delayed * 0.5f;
            
            // Mix wet and dry
            channelData[i] = input * (1.0f - reverbMix) + delayed * reverbMix;
            
            // Advance delay position
            m_effectsState.reverbPosition = 
                (m_effectsState.reverbPosition + 1) % m_effectsState.reverbBuffer.size();
        }
    }
}

void SamplerProcessor::processDistortion(AudioBuffer& buffer)
{
    // Simple waveshaping distortion
    const float distortionAmount = 5.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = channelData[i];
            
            // Soft clipping distortion
            float shaped = std::tanh(input * distortionAmount) / std::tanh(distortionAmount);
            
            channelData[i] = shaped;
        }
    }
}

void SamplerProcessor::processChorus(AudioBuffer& buffer)
{
    // Simple chorus effect (placeholder)
    // A real implementation would use modulated delay lines
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        // Placeholder - real chorus would modulate delay time
    }
}

bool SamplerProcessor::loadSampleFromPath(const wchar_t* path, bool resetParameters)
{
    if (!path || !canLoadFileExtension(path))
        return false;

    // Use the base class method to load the file
    bool success = Win32AudioProcessor::loadSampleFromFile(path);
    
    if (success)
    {
        m_currentFilePath = path;
        
        // Update synthesizer
        m_synth.setSampleBuffer(&m_sampleBuffer, m_bufferSampleRate.load());
        
        // Reset parameters if requested
        if (resetParameters)
        {
            setParameter(PARAM_SEMITONE, 0.0f);
            setParameter(PARAM_CENT, 0.0f);
            setParameter(PARAM_LOOP_START, 0.0f);
            setParameter(PARAM_LOOP_END, 1.0f);
        }
    }
    
    return success;
}

void SamplerProcessor::clearSample()
{
    m_sampleBuffer.setSize(2, 0);
    m_currentFilePath.clear();
    m_currentHash.clear();
}

void SamplerProcessor::haltVoices()
{
    // Stop all voices immediately
    m_synth.noteOff(0, 0, 0.0f, false);
}

void SamplerProcessor::playTestNote()
{
    // Play A5 (MIDI note 69)
    m_synth.noteOn(0, 69, 0.8f);
}

bool SamplerProcessor::canLoadFileExtension(const wchar_t* filePath) const
{
    if (!filePath)
        return false;
        
    std::wstring path(filePath);
    
    // Convert to lowercase for comparison
    std::transform(path.begin(), path.end(), path.begin(), ::towlower);
    
    // Check for supported extensions
    return (path.ends_with(L".wav") || 
            path.ends_with(L".mp3") || 
            path.ends_with(L".aiff") || 
            path.ends_with(L".flac") ||
            path.ends_with(L".ogg"));
}

bool SamplerProcessor::saveState(const wchar_t* filePath)
{
    // Placeholder for state saving
    // Would save parameters and optionally sample data to a file
    return false;
}

bool SamplerProcessor::loadState(const wchar_t* filePath)
{
    // Placeholder for state loading
    // Would load parameters and optionally sample data from a file
    return false;
}
