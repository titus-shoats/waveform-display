/*==============================================================================
    FL Studio SDK - Plugin Base Class
    Minimal version for POC purposes
==============================================================================*/

#pragma once

#include "fp_def.h"
#include "generictransport.h"

// Forward declarations
struct TFruityPlugInfo;
struct TFruityPlugHost;
struct TOutVoice;

// Plugin info structure
struct TFruityPlugInfo
{
    int SDKVersion;           // SDK version (current: 1)
    char* LongName;           // Full plugin name
    char* ShortName;          // Short plugin name  
    int Flags;                // Plugin flags (FPF_*)
    int NumParams;            // Number of parameters
    int DefPoly;              // Default polyphony
    int NumOutCtrls;          // Number of output controllers
    int NumOutVoices;         // Number of output voices
};

// Host callback structure
struct TFruityPlugHost
{
    int HostTag;
    int HostVersion;
    
    // Host callbacks (simplified for POC)
    void* (WINAPIV *Dispatcher)(void* Sender, int Index, int Value, void* Ptr);
    void (WINAPIV *OnIdle)(void* Sender);
    void (WINAPIV *OnHint)(void* Sender, int Index, const char* Hint);
    void (WINAPIV *OnParamChanged)(void* Sender, int Index, int Value);
    void (WINAPIV *OnControllerChanged)(void* Sender, int Index, int Value);
};

// Voice structure for output
struct TOutVoice
{
    int Delta;
    int Note;
    int Velocity;
    int InitLevels;
    int FineLevels;
    int Pan;
    int OutVoice;
};

// Base plugin class interface
class TFruityPlug
{
public:
    virtual ~TFruityPlug() {}
    
    // Core methods that must be implemented
    virtual void DestroyObject() = 0;
    virtual int Dispatcher(int Index, int Value, void* Ptr, int PtrSize) = 0;
    virtual void Idle() = 0;
    virtual void SaveRestoreState(void* Stream, int Save) = 0;
    
    // Audio processing
    virtual void Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length) = 0;
    
    // Parameter processing  
    virtual int ProcessParam(int Index, int Value, int Flags) = 0;
    
    // Optional methods
    virtual void GetName(int Index, int Value, char* Name) {}
    virtual int ProcessEvent(int EventID, int EventValue, int Flags) { return 0; }
    virtual void MIDITick(int* Tick) {}
    virtual void MIDIIn(int* Msg) {}
    virtual void Voice_Render(int VoiceIndex, PWAV32FS DestBuffer, int Length, int MaxLength) {}
    virtual void NewTick() {}
    virtual int Voice_ProcessEvent(int VoiceIndex, int EventID, int EventValue, int Flags) { return 0; }
    virtual void Voice_Release(int VoiceIndex) {}
    virtual void Voice_Kill(int VoiceIndex) {}
};

// Type definitions
typedef void* PWAV32FS;  // Pointer to float stereo audio buffer
