/*
  ==============================================================================

    SamplerProcessor.h
    Win32 API Sampler Processor (replaces JUCE JustaSampleAudioProcessor)
    
    This is the main audio processor for the Just a Sample Win32 application.

  ==============================================================================
*/

#pragma once

#include "AudioProcessor.h"
#include <string>

// Parameter indices
enum Parameters
{
    PARAM_SEMITONE = 0,
    PARAM_CENT,
    PARAM_WAVEFORM_SEMITONE,
    PARAM_WAVEFORM_CENT,
    PARAM_ATTACK_TIME,
    PARAM_ATTACK_CURVE,
    PARAM_RELEASE_TIME,
    PARAM_RELEASE_CURVE,
    PARAM_PLAYBACK_SPEED,
    PARAM_PLAYBACK_MODE,
    PARAM_LOFI_MODE,
    PARAM_LOOP_ENABLED,
    PARAM_LOOP_START,
    PARAM_LOOP_END,
    PARAM_MONO_OUTPUT,
    PARAM_GAIN,
    PARAM_EQ_ENABLED,
    PARAM_REVERB_ENABLED,
    PARAM_DISTORTION_ENABLED,
    PARAM_CHORUS_ENABLED,
    PARAM_COUNT
};

class SamplerProcessor : public Win32AudioProcessor
{
public:
    SamplerProcessor();
    ~SamplerProcessor() override;

    // Audio processing
    void processBlock(AudioBuffer& buffer, MidiBuffer& midiMessages) override;
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;

    // Sample management
    bool loadSampleFromPath(const wchar_t* path, bool resetParameters = true);
    void clearSample();
    
    // Sample playback control
    void haltVoices();
    void playTestNote();

    // File operations
    bool canLoadFileExtension(const wchar_t* filePath) const;
    
    // State management
    bool saveState(const wchar_t* filePath);
    bool loadState(const wchar_t* filePath);

    // Getters
    const AudioBuffer& getSampleBuffer() const { return m_sampleBuffer; }
    float getBufferSampleRate() const { return m_bufferSampleRate.load(); }
    
    // Parameter helpers
    void setSemitone(float value) { setParameter(PARAM_SEMITONE, value); }
    float getSemitone() const { return getParameter(PARAM_SEMITONE); }
    
    void setCent(float value) { setParameter(PARAM_CENT, value); }
    float getCent() const { return getParameter(PARAM_CENT); }
    
    void setAttackTime(float value) { setParameter(PARAM_ATTACK_TIME, value); }
    float getAttackTime() const { return getParameter(PARAM_ATTACK_TIME); }
    
    void setReleaseTime(float value) { setParameter(PARAM_RELEASE_TIME, value); }
    float getReleaseTime() const { return getParameter(PARAM_RELEASE_TIME); }
    
    void setGain(float value) { setParameter(PARAM_GAIN, value); }
    float getGain() const { return getParameter(PARAM_GAIN); }
    
    void setLoopEnabled(bool enabled) { setParameter(PARAM_LOOP_ENABLED, enabled ? 1.0f : 0.0f); }
    bool getLoopEnabled() const { return getParameter(PARAM_LOOP_ENABLED) > 0.5f; }

private:
    // Initialize synthesizer voices
    void initializeVoices(int numVoices = 8);
    
    // Effects processing
    void processEffects(AudioBuffer& buffer);
    void processEQ(AudioBuffer& buffer);
    void processReverb(AudioBuffer& buffer);
    void processDistortion(AudioBuffer& buffer);
    void processChorus(AudioBuffer& buffer);

    Synthesizer m_synth;
    
    // Effects state
    struct EffectsState
    {
        // Simple reverb state (placeholder)
        std::vector<float> reverbBuffer;
        int reverbPosition;
        
        // Simple distortion state
        float distortionAmount;
        
        // EQ state (simple 3-band)
        float lowGain;
        float midGain;
        float highGain;
        
        EffectsState() : reverbPosition(0), distortionAmount(0.0f),
                        lowGain(1.0f), midGain(1.0f), highGain(1.0f) {}
    };
    
    EffectsState m_effectsState;
    
    // File path tracking
    std::wstring m_currentFilePath;
    std::wstring m_currentHash;
};
