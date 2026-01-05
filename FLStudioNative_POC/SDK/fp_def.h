/*==============================================================================
    FL Studio SDK - Constants and Definitions
    Minimal version for POC purposes
==============================================================================*/

#pragma once

// Prevent Windows.h from defining min/max macros that conflict with JUCE
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

// Plugin types
#define FPT_Effect      0
#define FPT_VSTPlugin   1
#define FPT_Hybrid      2

// Dispatcher messages
#define FPD_SetEnabled          0
#define FPD_SetSampleRate       1
#define FPD_SetBlockSize        2
#define FPD_Reset               3
#define FPD_ShowEditor          4
#define FPD_ProcessMode         5
#define FPD_Flush               6
#define FPD_SetProgram          7
#define FPD_MIDIIn              8
#define FPD_MsgIn               9
#define FPD_MIDITick            10
#define FPD_MIDIOut             11
#define FPD_MsgOut              12
#define FPD_GetName             13
#define FPD_GetEventName        14
#define FPD_SetInternalController 15
#define FPD_GetNumParams        16
#define FPD_GetParamName        17
#define FPD_GetParamValue       18
#define FPD_ProcessParam        19
#define FPD_SetNumParams        20
#define FPD_Idle                21
#define FPD_LoadBank            22
#define FPD_SaveBank            23
#define FPD_WrapValue           24
#define FPD_SetSampleRate2      25
#define FPD_NewTick             26
#define FPD_GetInName           27
#define FPD_GetOutName          28
#define FPD_GetPresetName       29
#define FPD_SetPresetName       30
#define FPD_ShowSettings        31
#define FPD_EditSample          32
#define FPD_GetColor            33
#define FPD_SetColor            34
#define FPD_GetFocused          35
#define FPD_WantNewTick         36
#define FPD_GetEditorHandle     37
#define FPD_SetEnabled2         38
#define FPD_WantMIDIInput       39
#define FPD_SetSmoothSpeed      40
#define FPD_AfterProcessParam   41
#define FPD_SetTimeSig          42
#define FPD_CollectFile         43
#define FPD_TriggerVoice        44
#define FPD_SetInternalInput    45
#define FPD_GetPluginInfo       46
#define FPD_VoiceHandler        47
#define FPD_SetDemoMode         48
#define FPD_GetLatency          49
#define FPD_GetEditorSize       50
#define FPD_MIDIControlMessage  51

// Parameter processing flags
#define REC_UpdateValue     (1 << 0)
#define REC_UpdateControl   (1 << 1) 
#define REC_GetValue        (1 << 2)
#define REC_ShowHint        (1 << 3)
#define REC_UpdatePlugList  (1 << 4)
#define REC_SetChanged      (1 << 5)
#define REC_InternalController (1 << 6)
#define REC_PlugReserved    (1 << 7)
#define REC_FromMIDI        (1 << 8)
#define REC_ShowValue       (REC_UpdateValue | REC_ShowHint)

// Plugin flags
#define FPF_Type            0x0003
#define FPF_TimeFormat      0x000C
#define FPF_WantNewTick     0x0010
#define FPF_NewTickValue    0x0020
#define FPF_MsgOut          0x0040
#define FPF_MsgIn           0x0080
#define FPF_MIDIOut         0x0100
#define FPF_MIDIIn          0x0200
#define FPF_UsesSelectedChannel 0x0400
#define FPF_Generator       0x0800
#define FPF_WantNewTick2    0x1000
#define FPF_NewTickValue2   0x2000

// Voice handling flags
#define FHD_VoiceHandler    0
#define FHD_KillVoice       1
#define FHD_VoiceLevel      2
#define FHD_VoicePitch      3

// Max values
#define FPN_Param           65536  // Parameter range 0-65535
#define FPN_VoiceLevel      128
#define FPN_Voice           128

// Processing mode
#define PM_Suspended        0
#define PM_Oversampled      1

// Time format
#define FPT_Realtime        0
#define FPT_Song            1
