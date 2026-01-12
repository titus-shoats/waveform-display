/*
  ==============================================================================

    AudioProcessor.h
    Win32 API Audio Processor (replaces JUCE AudioProcessor)
    
    This class handles audio processing, MIDI input, and sample management
    using native Win32 APIs (WASAPI for audio, Win32 MIDI APIs).

  ==============================================================================
*/

#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <mmeapi.h>
#include <vector>
#include <memory>
#include <atomic>
#include <functional>

// Audio buffer structure
class AudioBuffer
{
public:
    AudioBuffer(int numChannels = 2, int numSamples = 0);
    ~AudioBuffer();

    void setSize(int numChannels, int numSamples, bool keepExistingContent = false);
    void clear();
    
    float* getWritePointer(int channel);
    const float* getReadPointer(int channel) const;
    
    int getNumChannels() const { return m_numChannels; }
    int getNumSamples() const { return m_numSamples; }

private:
    std::vector<std::vector<float>> m_data;
    int m_numChannels;
    int m_numSamples;
};

// MIDI message structure
struct MidiMessage
{
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
    DWORD timestamp;

    bool isNoteOn() const { return (status & 0xF0) == 0x90 && data2 > 0; }
    bool isNoteOff() const { return (status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0); }
    int getNoteNumber() const { return data1; }
    int getVelocity() const { return data2; }
};

class MidiBuffer
{
public:
    void addEvent(const MidiMessage& message);
    void clear();
    
    const std::vector<MidiMessage>& getEvents() const { return m_events; }

private:
    std::vector<MidiMessage> m_events;
};

// Main audio processor class (Win32 equivalent of JUCE AudioProcessor)
class Win32AudioProcessor
{
public:
    Win32AudioProcessor();
    virtual ~Win32AudioProcessor();

    // Audio device management
    bool initializeAudio(int sampleRate = 44100, int bufferSize = 512);
    void releaseAudio();
    bool isAudioInitialized() const { return m_audioInitialized; }

    // MIDI device management
    bool initializeMidi();
    void releaseMidi();

    // Audio processing callback (to be implemented by derived classes)
    virtual void processBlock(AudioBuffer& buffer, MidiBuffer& midiMessages) = 0;

    // Prepare for playback
    virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock);
    virtual void releaseResources();

    // Sample management
    bool loadSampleFromFile(const wchar_t* filePath);
    const AudioBuffer& getSampleBuffer() const { return m_sampleBuffer; }
    float getSampleRate() const { return m_sampleRate; }

    // Playback control
    void startPlayback();
    void stopPlayback();
    bool isPlaying() const { return m_isPlaying; }

    // Parameter management
    void setParameter(int index, float value);
    float getParameter(int index) const;

protected:
    // Audio rendering thread function
    static DWORD WINAPI AudioThreadProc(LPVOID lpParam);
    void audioThreadRun();

    // MIDI callback
    static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, 
                                    DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    // Audio device interfaces (WASAPI)
    IMMDeviceEnumerator* m_deviceEnumerator;
    IMMDevice* m_device;
    IAudioClient* m_audioClient;
    IAudioRenderClient* m_renderClient;

    // MIDI device
    HMIDIIN m_hMidiIn;
    MidiBuffer m_midiBuffer;
    CRITICAL_SECTION m_midiLock;

    // Audio thread
    HANDLE m_audioThread;
    HANDLE m_stopEvent;
    std::atomic<bool> m_isPlaying;
    std::atomic<bool> m_audioInitialized;

    // Audio parameters
    float m_sampleRate;
    int m_bufferSize;
    int m_numChannels;

    // Sample data
    AudioBuffer m_sampleBuffer;
    std::atomic<float> m_bufferSampleRate;

    // Parameters (simple float array for now)
    std::vector<std::atomic<float>> m_parameters;
};

// Sampler voice class (Win32 equivalent of JUCE SamplerVoice)
class SamplerVoice
{
public:
    SamplerVoice();
    ~SamplerVoice();

    void startNote(int midiNoteNumber, float velocity, const AudioBuffer* sampleBuffer, float sampleRate);
    void stopNote(float velocity, bool allowTailOff);
    
    bool isActive() const { return m_isActive; }
    void renderNextBlock(AudioBuffer& outputBuffer, int startSample, int numSamples);

private:
    bool m_isActive;
    double m_currentPosition;
    double m_pitchRatio;
    int m_currentNote;
    float m_currentVelocity;
    const AudioBuffer* m_sampleToPlay;
    float m_sourceSampleRate;
    float m_outputSampleRate;
    
    // ADSR envelope
    float m_attackTime;
    float m_releaseTime;
    float m_envelopeValue;
    bool m_isReleasing;
};

// Synthesizer class (Win32 equivalent of JUCE Synthesiser)
class Synthesizer
{
public:
    Synthesizer();
    ~Synthesizer();

    void addVoice(SamplerVoice* voice);
    void clearVoices();

    void setSampleBuffer(const AudioBuffer* buffer, float sampleRate);

    void noteOn(int midiChannel, int midiNoteNumber, float velocity);
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);

    void renderNextBlock(AudioBuffer& outputBuffer, const MidiBuffer& midiMessages);

private:
    std::vector<std::unique_ptr<SamplerVoice>> m_voices;
    const AudioBuffer* m_currentSample;
    float m_currentSampleRate;
    CRITICAL_SECTION m_voiceLock;
};
