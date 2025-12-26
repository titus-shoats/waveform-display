/*==============================================================================
    PluginEditor.h
    FL Studio Native + JUCE GUI POC - Editor Component
    
    This demonstrates how to create a JUCE GUI component that can be embedded
    in an FL Studio plugin window. The editor includes various JUCE components
    to prove that timers, buttons, sliders, and file dialogs work correctly
    when embedded in FL Studio.
==============================================================================*/

#pragma once

// JUCE 7+ module headers (instead of monolithic JuceHeader.h)
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

// Forward declaration
class HelloWorldProcessor;

class HelloWorldEditor : public juce::Component
                       , public juce::Timer
{
public:
    HelloWorldEditor(HelloWorldProcessor& processor);
    ~HelloWorldEditor() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback
    void timerCallback() override;
    
private:
    HelloWorldProcessor& processor;
    
    // UI Components
    juce::Label titleLabel;
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Label gainValueLabel;
    juce::TextButton testButton;
    juce::Label timerLabel;
    juce::TextButton fileChooserButton;
    juce::Label statusLabel;
    
    // Timer counter
    int timerCounter{0};
    
    // Button callbacks
    void onTestButtonClicked();
    void onFileChooserButtonClicked();
    void onGainSliderChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelloWorldEditor)
};
