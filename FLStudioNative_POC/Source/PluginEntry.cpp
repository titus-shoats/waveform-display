/*==============================================================================
    PluginEntry.cpp
    FL Studio Native + JUCE GUI POC - Plugin Entry Point
    
    This file provides the DLL export that FL Studio calls to create plugin
    instances. It's responsible for:
    - Initializing JUCE on first load
    - Creating processor instances
    
    FL Studio expects a function with this signature:
    TFruityPlug* _stdcall CreatePlugInstance(TFruityPlugHost* Host, int Tag)
==============================================================================*/

#include "PluginProcessor.h"

// Static flag to track JUCE initialization
static bool g_juceInitialized = false;

extern "C"
{
    /**
     * CreatePlugInstance - FL Studio plugin entry point
     * 
     * This function is called by FL Studio to create a new plugin instance.
     * It must be exported with C linkage and _stdcall calling convention.
     * 
     * @param Host - Pointer to FL Studio host callback structure
     * @param Tag  - Unique tag for this plugin instance
     * @return Pointer to new plugin instance
     */
    __declspec(dllexport) TFruityPlug* _stdcall CreatePlugInstance(TFruityPlugHost* Host, int Tag)
    {
        // Initialize JUCE on first plugin load
        // This sets up the message manager and other JUCE subsystems
        if (!g_juceInitialized)
        {
            juce::initialiseJuce_GUI();
            g_juceInitialized = true;
        }
        
        // Create and return new processor instance
        // FL Studio will call methods on this instance to control the plugin
        return new HelloWorldProcessor(Tag, Host);
    }
}
