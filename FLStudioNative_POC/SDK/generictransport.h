/*==============================================================================
    FL Studio SDK - Generic Transport Info
    Minimal version for POC purposes
==============================================================================*/

#pragma once

struct TGenericTransport
{
    // Transport state
    int Playing;          // 1 if playing
    int Recording;        // 1 if recording
    int LoopActive;       // 1 if loop active
    
    // Timing
    double SamplePos;     // Current position in samples
    double SongPos;       // Song position in beats
    double BarPos;        // Position within current bar
    
    // Tempo and time signature
    double Tempo;         // BPM
    int TimeSigNum;       // Time signature numerator (e.g., 4 in 4/4)
    int TimeSigDen;       // Time signature denominator (e.g., 4 in 4/4)
    
    // Loop markers
    double LoopStart;     // Loop start in beats
    double LoopEnd;       // Loop end in beats
};
