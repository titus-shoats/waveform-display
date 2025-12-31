/*
  ==============================================================================

    AudioProcessor.cpp
    Win32 API Audio Processor Implementation

  ==============================================================================
*/

#include "AudioProcessor.h"
#include <algorithm>
#include <cmath>

// AudioBuffer implementation
AudioBuffer::AudioBuffer(int numChannels, int numSamples)
    : m_numChannels(numChannels), m_numSamples(numSamples)
{
    setSize(numChannels, numSamples);
}

AudioBuffer::~AudioBuffer()
{
}

void AudioBuffer::setSize(int numChannels, int numSamples, bool keepExistingContent)
{
    m_numChannels = numChannels;
    m_numSamples = numSamples;
    
    if (!keepExistingContent)
    {
        m_data.clear();
        m_data.resize(numChannels);
        for (int i = 0; i < numChannels; ++i)
        {
            m_data[i].resize(numSamples, 0.0f);
        }
    }
    else
    {
        m_data.resize(numChannels);
        for (int i = 0; i < numChannels; ++i)
        {
            m_data[i].resize(numSamples, 0.0f);
        }
    }
}

void AudioBuffer::clear()
{
    for (auto& channel : m_data)
    {
        std::fill(channel.begin(), channel.end(), 0.0f);
    }
}

float* AudioBuffer::getWritePointer(int channel)
{
    if (channel >= 0 && channel < m_numChannels)
        return m_data[channel].data();
    return nullptr;
}

const float* AudioBuffer::getReadPointer(int channel) const
{
    if (channel >= 0 && channel < m_numChannels)
        return m_data[channel].data();
    return nullptr;
}

// MidiBuffer implementation
void MidiBuffer::addEvent(const MidiMessage& message)
{
    m_events.push_back(message);
}

void MidiBuffer::clear()
{
    m_events.clear();
}

// Win32AudioProcessor implementation
Win32AudioProcessor::Win32AudioProcessor()
    : m_deviceEnumerator(nullptr)
    , m_device(nullptr)
    , m_audioClient(nullptr)
    , m_renderClient(nullptr)
    , m_hMidiIn(nullptr)
    , m_audioThread(nullptr)
    , m_stopEvent(nullptr)
    , m_isPlaying(false)
    , m_audioInitialized(false)
    , m_sampleRate(44100.0f)
    , m_bufferSize(512)
    , m_numChannels(2)
    , m_bufferSampleRate(44100.0f)
{
    InitializeCriticalSection(&m_midiLock);
    m_parameters.resize(100);  // Allocate space for parameters
    for (auto& param : m_parameters)
        param.store(0.0f);
}

Win32AudioProcessor::~Win32AudioProcessor()
{
    releaseAudio();
    releaseMidi();
    DeleteCriticalSection(&m_midiLock);
}

bool Win32AudioProcessor::initializeAudio(int sampleRate, int bufferSize)
{
    HRESULT hr;

    // Initialize COM
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
        return false;

    // Create device enumerator
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                         __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
    if (FAILED(hr))
        return false;

    // Get default audio endpoint
    hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
    if (FAILED(hr))
        return false;

    // Activate audio client
    hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_audioClient);
    if (FAILED(hr))
        return false;

    // Get mix format
    WAVEFORMATEX* pwfx = nullptr;
    hr = m_audioClient->GetMixFormat(&pwfx);
    if (FAILED(hr))
        return false;

    m_sampleRate = static_cast<float>(pwfx->nSamplesPerSec);
    m_numChannels = pwfx->nChannels;

    // Initialize audio client
    REFERENCE_TIME requestedDuration = 10000000LL * bufferSize / pwfx->nSamplesPerSec;
    hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, requestedDuration, 0, pwfx, nullptr);
    CoTaskMemFree(pwfx);
    
    if (FAILED(hr))
        return false;

    // Get buffer size
    UINT32 actualBufferSize;
    hr = m_audioClient->GetBufferSize(&actualBufferSize);
    if (FAILED(hr))
        return false;

    m_bufferSize = actualBufferSize;

    // Get render client
    hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_renderClient);
    if (FAILED(hr))
        return false;

    // Create stop event
    m_stopEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_stopEvent)
        return false;

    // Prepare to play
    prepareToPlay(m_sampleRate, m_bufferSize);

    m_audioInitialized.store(true);
    return true;
}

void Win32AudioProcessor::releaseAudio()
{
    stopPlayback();

    if (m_audioClient)
    {
        m_audioClient->Stop();
    }

    if (m_stopEvent)
    {
        CloseHandle(m_stopEvent);
        m_stopEvent = nullptr;
    }

    if (m_renderClient)
    {
        m_renderClient->Release();
        m_renderClient = nullptr;
    }

    if (m_audioClient)
    {
        m_audioClient->Release();
        m_audioClient = nullptr;
    }

    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_deviceEnumerator)
    {
        m_deviceEnumerator->Release();
        m_deviceEnumerator = nullptr;
    }

    m_audioInitialized.store(false);
    CoUninitialize();
}

bool Win32AudioProcessor::initializeMidi()
{
    // Get number of MIDI input devices
    UINT numDevices = midiInGetNumDevs();
    if (numDevices == 0)
        return false;

    // Open first MIDI input device
    MMRESULT result = midiInOpen(&m_hMidiIn, 0, (DWORD_PTR)MidiInProc, 
                                 (DWORD_PTR)this, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR)
        return false;

    // Start MIDI input
    result = midiInStart(m_hMidiIn);
    if (result != MMSYSERR_NOERROR)
    {
        midiInClose(m_hMidiIn);
        m_hMidiIn = nullptr;
        return false;
    }

    return true;
}

void Win32AudioProcessor::releaseMidi()
{
    if (m_hMidiIn)
    {
        midiInStop(m_hMidiIn);
        midiInClose(m_hMidiIn);
        m_hMidiIn = nullptr;
    }
}

void Win32AudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Override in derived class
}

void Win32AudioProcessor::releaseResources()
{
    // Override in derived class
}

bool Win32AudioProcessor::loadSampleFromFile(const wchar_t* filePath)
{
    // This is a placeholder - actual implementation would use
    // Win32 APIs to read WAV/audio files
    // For now, just return false
    return false;
}

void Win32AudioProcessor::startPlayback()
{
    if (!m_audioInitialized.load())
        return;

    if (m_isPlaying.load())
        return;

    // Start audio client
    HRESULT hr = m_audioClient->Start();
    if (FAILED(hr))
        return;

    // Create audio thread
    m_audioThread = CreateThread(nullptr, 0, AudioThreadProc, this, 0, nullptr);
    if (!m_audioThread)
    {
        m_audioClient->Stop();
        return;
    }

    m_isPlaying.store(true);
}

void Win32AudioProcessor::stopPlayback()
{
    if (!m_isPlaying.load())
        return;

    m_isPlaying.store(false);

    // Signal stop event
    if (m_stopEvent)
        SetEvent(m_stopEvent);

    // Wait for thread to finish
    if (m_audioThread)
    {
        WaitForSingleObject(m_audioThread, INFINITE);
        CloseHandle(m_audioThread);
        m_audioThread = nullptr;
    }

    if (m_audioClient)
        m_audioClient->Stop();
}

void Win32AudioProcessor::setParameter(int index, float value)
{
    if (index >= 0 && index < static_cast<int>(m_parameters.size()))
        m_parameters[index].store(value);
}

float Win32AudioProcessor::getParameter(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_parameters.size()))
        return m_parameters[index].load();
    return 0.0f;
}

DWORD WINAPI Win32AudioProcessor::AudioThreadProc(LPVOID lpParam)
{
    Win32AudioProcessor* processor = static_cast<Win32AudioProcessor*>(lpParam);
    processor->audioThreadRun();
    return 0;
}

void Win32AudioProcessor::audioThreadRun()
{
    // Audio rendering loop
    AudioBuffer renderBuffer(m_numChannels, m_bufferSize);
    MidiBuffer midiBuffer;

    while (m_isPlaying.load())
    {
        // Wait for buffer to be ready
        DWORD waitResult = WaitForSingleObject(m_stopEvent, 10);
        if (waitResult == WAIT_OBJECT_0)
            break;

        // Get available buffer space
        UINT32 padding;
        HRESULT hr = m_audioClient->GetCurrentPadding(&padding);
        if (FAILED(hr))
            continue;

        UINT32 availableFrames = m_bufferSize - padding;
        if (availableFrames == 0)
            continue;

        // Get buffer
        BYTE* data;
        hr = m_renderClient->GetBuffer(availableFrames, &data);
        if (FAILED(hr))
            continue;

        // Process audio
        renderBuffer.setSize(m_numChannels, availableFrames);
        renderBuffer.clear();
        
        // Get MIDI events
        EnterCriticalSection(&m_midiLock);
        MidiBuffer currentMidi = m_midiBuffer;
        m_midiBuffer.clear();
        LeaveCriticalSection(&m_midiLock);

        // Call derived class processing
        processBlock(renderBuffer, currentMidi);

        // Interleave audio data
        float* floatData = reinterpret_cast<float*>(data);
        for (UINT32 i = 0; i < availableFrames; ++i)
        {
            for (int ch = 0; ch < m_numChannels; ++ch)
            {
                floatData[i * m_numChannels + ch] = renderBuffer.getReadPointer(ch)[i];
            }
        }

        // Release buffer
        m_renderClient->ReleaseBuffer(availableFrames, 0);
    }
}

void CALLBACK Win32AudioProcessor::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance,
                                              DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (wMsg != MIM_DATA)
        return;

    Win32AudioProcessor* processor = reinterpret_cast<Win32AudioProcessor*>(dwInstance);
    
    MidiMessage message;
    message.status = static_cast<unsigned char>(dwParam1 & 0xFF);
    message.data1 = static_cast<unsigned char>((dwParam1 >> 8) & 0xFF);
    message.data2 = static_cast<unsigned char>((dwParam1 >> 16) & 0xFF);
    message.timestamp = static_cast<DWORD>(dwParam2);

    EnterCriticalSection(&processor->m_midiLock);
    processor->m_midiBuffer.addEvent(message);
    LeaveCriticalSection(&processor->m_midiLock);
}

// SamplerVoice implementation
SamplerVoice::SamplerVoice()
    : m_isActive(false)
    , m_currentPosition(0.0)
    , m_pitchRatio(1.0)
    , m_currentNote(0)
    , m_currentVelocity(0.0f)
    , m_sampleToPlay(nullptr)
    , m_sourceSampleRate(44100.0f)
    , m_outputSampleRate(44100.0f)
    , m_attackTime(0.01f)
    , m_releaseTime(0.1f)
    , m_envelopeValue(0.0f)
    , m_isReleasing(false)
{
}

SamplerVoice::~SamplerVoice()
{
}

void SamplerVoice::startNote(int midiNoteNumber, float velocity, const AudioBuffer* sampleBuffer, float sampleRate)
{
    if (!sampleBuffer || sampleBuffer->getNumSamples() == 0)
        return;

    m_currentNote = midiNoteNumber;
    m_currentVelocity = velocity;
    m_sampleToPlay = sampleBuffer;
    m_sourceSampleRate = sampleRate;
    m_currentPosition = 0.0;
    m_isActive = true;
    m_isReleasing = false;
    m_envelopeValue = 0.0f;

    // Calculate pitch ratio (assuming middle C = 60)
    double pitchShift = (midiNoteNumber - 60) / 12.0;
    m_pitchRatio = std::pow(2.0, pitchShift);
}

void SamplerVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_isReleasing = true;
    }
    else
    {
        m_isActive = false;
    }
}

void SamplerVoice::renderNextBlock(AudioBuffer& outputBuffer, int startSample, int numSamples)
{
    if (!m_isActive || !m_sampleToPlay)
        return;

    int endSample = startSample + numSamples;
    int sampleBufferLength = m_sampleToPlay->getNumSamples();

    for (int i = startSample; i < endSample; ++i)
    {
        if (m_currentPosition >= sampleBufferLength)
        {
            m_isActive = false;
            break;
        }

        // Simple linear interpolation for resampling
        int pos = static_cast<int>(m_currentPosition);
        float frac = static_cast<float>(m_currentPosition - pos);
        
        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        {
            float sample = 0.0f;
            
            if (pos < sampleBufferLength)
            {
                int sourceChannel = std::min(ch, m_sampleToPlay->getNumChannels() - 1);
                const float* sourceData = m_sampleToPlay->getReadPointer(sourceChannel);
                
                sample = sourceData[pos];
                if (pos + 1 < sampleBufferLength)
                {
                    sample += frac * (sourceData[pos + 1] - sourceData[pos]);
                }
            }

            // Apply envelope
            if (!m_isReleasing)
            {
                m_envelopeValue = std::min(1.0f, m_envelopeValue + 1.0f / (m_attackTime * m_outputSampleRate));
            }
            else
            {
                m_envelopeValue = std::max(0.0f, m_envelopeValue - 1.0f / (m_releaseTime * m_outputSampleRate));
                if (m_envelopeValue <= 0.0f)
                {
                    m_isActive = false;
                    break;
                }
            }

            float* outputData = outputBuffer.getWritePointer(ch);
            outputData[i] += sample * m_envelopeValue * m_currentVelocity;
        }

        m_currentPosition += m_pitchRatio * (m_sourceSampleRate / m_outputSampleRate);
    }
}

// Synthesizer implementation
Synthesizer::Synthesizer()
    : m_currentSample(nullptr)
    , m_currentSampleRate(44100.0f)
{
    InitializeCriticalSection(&m_voiceLock);
}

Synthesizer::~Synthesizer()
{
    DeleteCriticalSection(&m_voiceLock);
}

void Synthesizer::addVoice(SamplerVoice* voice)
{
    m_voices.push_back(std::unique_ptr<SamplerVoice>(voice));
}

void Synthesizer::clearVoices()
{
    EnterCriticalSection(&m_voiceLock);
    m_voices.clear();
    LeaveCriticalSection(&m_voiceLock);
}

void Synthesizer::setSampleBuffer(const AudioBuffer* buffer, float sampleRate)
{
    m_currentSample = buffer;
    m_currentSampleRate = sampleRate;
}

void Synthesizer::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    EnterCriticalSection(&m_voiceLock);
    
    // Find inactive voice
    for (auto& voice : m_voices)
    {
        if (!voice->isActive())
        {
            voice->startNote(midiNoteNumber, velocity, m_currentSample, m_currentSampleRate);
            break;
        }
    }
    
    LeaveCriticalSection(&m_voiceLock);
}

void Synthesizer::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
{
    EnterCriticalSection(&m_voiceLock);
    
    for (auto& voice : m_voices)
    {
        if (voice->isActive())
        {
            voice->stopNote(velocity, allowTailOff);
        }
    }
    
    LeaveCriticalSection(&m_voiceLock);
}

void Synthesizer::renderNextBlock(AudioBuffer& outputBuffer, const MidiBuffer& midiMessages)
{
    // Process MIDI messages
    for (const auto& message : midiMessages.getEvents())
    {
        if (message.isNoteOn())
        {
            noteOn(0, message.getNoteNumber(), message.getVelocity() / 127.0f);
        }
        else if (message.isNoteOff())
        {
            noteOff(0, message.getNoteNumber(), message.getVelocity() / 127.0f, true);
        }
    }

    // Render all voices
    EnterCriticalSection(&m_voiceLock);
    
    for (auto& voice : m_voices)
    {
        if (voice->isActive())
        {
            voice->renderNextBlock(outputBuffer, 0, outputBuffer.getNumSamples());
        }
    }
    
    LeaveCriticalSection(&m_voiceLock);
}
