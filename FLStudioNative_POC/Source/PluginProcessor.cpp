/*==============================================================================
    PluginProcessor.cpp
    FL Studio Native + JUCE GUI POC - Processor Implementation
    
    This implements the FL Studio plugin interface and demonstrates:
    - Creating Win32 container window
    - Embedding JUCE editor via addToDesktop()
    - Pumping JUCE message loop in Idle()
    - Parameter bridging between FL Studio and JUCE
    - Simple gain audio effect
==============================================================================*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// Parameter indices
enum Parameters
{
    Param_Gain = 0,
    Param_Pan = 1,
    Param_Bypass = 2,
    Param_Count
};

HelloWorldProcessor::HelloWorldProcessor(int PlugTag, TFruityPlugHost* PlugHost)
    : TCPPFruityPlug(PlugTag, PlugHost)
{
    // Initialize plugin info
    std::strncpy(Info.LongName, "FL Studio Native + JUCE GUI POC", 255);
    Info.LongName[255] = '\0';  // Ensure null termination
    std::strncpy(Info.ShortName, "JUCE POC", 63);
    Info.ShortName[63] = '\0';  // Ensure null termination
    Info.NumParams = Param_Count;
    Info.Flags = FPF_Type | FPF_Generator;  // Effect plugin
    
    // Initialize JUCE message manager if not already initialized
    // Note: This is done in PluginEntry.cpp, but we check here too
    if (!juce::MessageManager::getInstanceWithoutCreating())
    {
        juce::initialiseJuce_GUI();
    }
}

HelloWorldProcessor::~HelloWorldProcessor()
{
    destroyEditorWindow();
}

void HelloWorldProcessor::DestroyObject()
{
    delete this;
}

int HelloWorldProcessor::Dispatcher(int Index, int Value, void* Ptr, int PtrSize)
{
    switch (Index)
    {
        case FPD_SetSampleRate:
        {
            // Set sample rate
            sampleRate = *static_cast<int*>(Ptr);
            return 0;
        }
        
        case FPD_ShowEditor:
        {
            // Show or hide editor
            if (Value == 0)
            {
                // Hide editor
                destroyEditorWindow();
            }
            else
            {
                // Show editor - Ptr contains parent HWND
                HWND parentWindow = static_cast<HWND>(Ptr);
                createEditorWindow(parentWindow);
            }
            return 0;
        }
        
        case FPD_GetEditorSize:
        {
            // Return editor size (width in low word, height in high word)
            if (juceEditor)
            {
                int width = juceEditor->getWidth();
                int height = juceEditor->getHeight();
                return (height << 16) | width;
            }
            return (400 << 16) | 600;  // Default size
        }
        
        case FPD_GetPluginInfo:
        {
            // Return plugin info structure
            return reinterpret_cast<int>(&Info);
        }
        
        case FPD_Reset:
        {
            // Reset plugin state
            gain = 0.8f;
            pan = 0.5f;
            bypass = false;
            return 0;
        }
        
        default:
            return 0;
    }
}

void HelloWorldProcessor::Idle()
{
    // Pump JUCE message loop
    // This is critical for JUCE GUI to work - it processes events, timers, etc.
    if (juce::MessageManager::getInstanceWithoutCreating())
    {
        juce::MessageManager::getInstance()->runDispatchLoopUntil(1);
    }
}

void HelloWorldProcessor::SaveRestoreState(void* Stream, int Save)
{
    // For POC, we'll just save/load the gain parameter
    // In a real plugin, you'd use a proper serialization method
    
    if (Save)
    {
        // Save state
        // Stream would be a file handle or memory buffer
        // For POC purposes, this is a placeholder
    }
    else
    {
        // Load state
        // Read from Stream and restore parameters
    }
}

void HelloWorldProcessor::Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length)
{
    // Simple gain effect
    // FL Studio audio buffers are interleaved stereo float arrays
    
    if (bypass)
    {
        // Bypass - just copy input to output
        std::memcpy(DestBuffer, SourceBuffer, Length * 2 * sizeof(float));
        return;
    }
    
    float* src = static_cast<float*>(SourceBuffer);
    float* dst = static_cast<float*>(DestBuffer);
    
    // Apply gain and pan
    float leftGain = gain * (1.0f - pan * 0.5f);
    float rightGain = gain * (1.0f - (1.0f - pan) * 0.5f);
    
    for (int i = 0; i < Length; ++i)
    {
        dst[i * 2] = src[i * 2] * leftGain;        // Left channel
        dst[i * 2 + 1] = src[i * 2 + 1] * rightGain;  // Right channel
    }
}

int HelloWorldProcessor::ProcessParam(int Index, int Value, int Flags)
{
    // Parameter processing - bridge between FL Studio (0-65535) and JUCE (0.0-1.0)
    
    if (Flags & REC_GetValue)
    {
        // Get current parameter value
        switch (Index)
        {
            case Param_Gain:
                return normalizedToFLParam(gain);
            case Param_Pan:
                return normalizedToFLParam(pan);
            case Param_Bypass:
                return bypass ? 65535 : 0;
            default:
                return 0;
        }
    }
    
    if (Flags & REC_UpdateValue)
    {
        // Update parameter value
        switch (Index)
        {
            case Param_Gain:
                gain = flParamToNormalized(Value);
                break;
            case Param_Pan:
                pan = flParamToNormalized(Value);
                break;
            case Param_Bypass:
                bypass = (Value > 32767);
                break;
        }
        
        // Notify host of change
        NotifyHostParamChanged(Index, Value);
    }
    
    if (Flags & REC_ShowHint)
    {
        // Show parameter hint
        char hint[256];
        switch (Index)
        {
            case Param_Gain:
                std::snprintf(hint, sizeof(hint), "Gain: %.2f", gain);
                break;
            case Param_Pan:
                std::snprintf(hint, sizeof(hint), "Pan: %.2f", pan);
                break;
            case Param_Bypass:
                std::snprintf(hint, sizeof(hint), "Bypass: %s", bypass ? "On" : "Off");
                break;
            default:
                hint[0] = '\0';
        }
        ShowHint(Index, hint);
    }
    
    return 0;
}

void HelloWorldProcessor::setGainParameter(float normalizedValue)
{
    gain = juce::jlimit(0.0f, 1.0f, normalizedValue);
    
    // Notify FL Studio of parameter change
    int flValue = normalizedToFLParam(gain);
    NotifyHostParamChanged(Param_Gain, flValue);
}

void HelloWorldProcessor::createEditorWindow(HWND parentWindow)
{
    if (juceEditor)
        return;  // Already created
    
    // Create Win32 container window
    // This window will host the JUCE editor
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"JUCEPOCContainer";
    
    static bool registered = false;
    if (!registered)
    {
        RegisterClassEx(&wc);
        registered = true;
    }
    
    // Create the container window
    containerWindow = CreateWindowEx(
        0,
        L"JUCEPOCContainer",
        L"JUCE Editor Container",
        WS_CHILD | WS_VISIBLE,
        0, 0, 600, 400,
        parentWindow,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!containerWindow)
    {
        // Failed to create window - log error for debugging
        DWORD error = GetLastError();
        // TODO: Add proper error logging
        // For now, just return gracefully
        return;
    }
    
    // Create JUCE editor
    juceEditor = std::make_unique<HelloWorldEditor>(*this);
    
    // Embed JUCE editor in Win32 window
    // The magic happens here: addToDesktop() with the container window as parent
    juceEditor->addToDesktop(0, containerWindow);
    juceEditor->setVisible(true);
}

void HelloWorldProcessor::destroyEditorWindow()
{
    if (juceEditor)
    {
        juceEditor->removeFromDesktop();
        juceEditor.reset();
    }
    
    if (containerWindow)
    {
        DestroyWindow(containerWindow);
        containerWindow = nullptr;
    }
}

int HelloWorldProcessor::normalizedToFLParam(float normalized) const
{
    return static_cast<int>(normalized * 65535.0f);
}

float HelloWorldProcessor::flParamToNormalized(int flParam) const
{
    return static_cast<float>(flParam) / 65535.0f;
}

LRESULT CALLBACK HelloWorldProcessor::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            return 0;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
