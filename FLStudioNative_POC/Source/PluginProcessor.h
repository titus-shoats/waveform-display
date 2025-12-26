/*==============================================================================
    PluginProcessor.h
    FL Studio Native + JUCE GUI POC - Processor
    
    This demonstrates the hybrid architecture where:
    1. FL Studio SDK provides the plugin interface (TCPPFruityPlug)
    2. Win32 provides the container window
    3. JUCE provides the GUI components
    
    Key responsibilities:
    - Implement FL Studio plugin interface
    - Create Win32 container window for JUCE editor
    - Bridge parameters between FL Studio (0-65535 int) and JUCE (0.0-1.0 float)
    - Pump JUCE message loop via Idle()
    - Process audio with simple gain effect
==============================================================================*/

#pragma once

#include "../SDK/fp_cplug.h"

// JUCE 7+ module headers (instead of monolithic JuceHeader.h)
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>

// Forward declaration
class HelloWorldEditor;

class HelloWorldProcessor : public TCPPFruityPlug
{
public:
    HelloWorldProcessor(int PlugTag, TFruityPlugHost* PlugHost);
    ~HelloWorldProcessor() override;
    
    // FL Studio plugin interface - Core methods
    void DestroyObject() override;
    int Dispatcher(int Index, int Value, void* Ptr, int PtrSize) override;
    void Idle() override;
    void SaveRestoreState(void* Stream, int Save) override;
    void Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length) override;
    int ProcessParam(int Index, int Value, int Flags) override;
    
    // Parameter bridge methods (called from JUCE editor)
    void setGainParameter(float normalizedValue);
    float getGainParameter() const { return gain; }
    
private:
    // Win32 window handle for container
    HWND containerWindow{nullptr};
    
    // JUCE editor instance
    std::unique_ptr<HelloWorldEditor> juceEditor;
    
    // Plugin parameters (normalized 0.0-1.0)
    float gain{0.8f};
    float pan{0.5f};
    bool bypass{false};
    
    // Sample rate
    double sampleRate{44100.0};
    
    // Helper methods
    void createEditorWindow(HWND parentWindow);
    void destroyEditorWindow();
    int normalizedToFLParam(float normalized) const;
    float flParamToNormalized(int flParam) const;
    
    // Window procedure
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelloWorldProcessor)
};
