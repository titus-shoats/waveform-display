/*==============================================================================
    PluginEditor.cpp
    FL Studio Native + JUCE GUI POC - Editor Implementation
    
    This implements a JUCE GUI that demonstrates:
    - Timer updates (proves JUCE message loop integration works)
    - Button interactions (proves event handling works)
    - Slider parameter changes (proves FL Studio ↔ JUCE bridge works)
    - File chooser dialogs (proves modal dialogs work)
==============================================================================*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

HelloWorldEditor::HelloWorldEditor(HelloWorldProcessor& proc)
    : processor(proc)
{
    // Set editor size
    setSize(600, 400);
    
    // Title Label
    titleLabel.setText("FL Studio Native + JUCE GUI POC", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Gain Slider
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setValue(0.8);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    gainSlider.setColour(juce::Slider::trackColourId, juce::Colours::green);
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightgreen);
    gainSlider.onValueChange = [this] { onGainSliderChanged(); };
    addAndMakeVisible(gainSlider);
    
    // Gain Label
    gainLabel.setText("Gain:", juce::dontSendNotification);
    gainLabel.setFont(juce::Font(16.0f));
    gainLabel.setJustificationType(juce::Justification::centredLeft);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(gainLabel);
    
    // Gain Value Label
    gainValueLabel.setText("0.80", juce::dontSendNotification);
    gainValueLabel.setFont(juce::Font(14.0f));
    gainValueLabel.setJustificationType(juce::Justification::centredRight);
    gainValueLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    addAndMakeVisible(gainValueLabel);
    
    // Test Button
    testButton.setButtonText("Click Me!");
    testButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    testButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    testButton.onClick = [this] { onTestButtonClicked(); };
    addAndMakeVisible(testButton);
    
    // Timer Label
    timerLabel.setText("Timer: 0s", juce::dontSendNotification);
    timerLabel.setFont(juce::Font(16.0f));
    timerLabel.setJustificationType(juce::Justification::centred);
    timerLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    addAndMakeVisible(timerLabel);
    
    // File Chooser Button
    fileChooserButton.setButtonText("Open File Dialog");
    fileChooserButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
    fileChooserButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    fileChooserButton.onClick = [this] { onFileChooserButtonClicked(); };
    addAndMakeVisible(fileChooserButton);
    
    // Status Label
    statusLabel.setText("Status: Ready", juce::dontSendNotification);
    statusLabel.setFont(juce::Font(14.0f));
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(statusLabel);
    
    // Start timer (updates every 1000ms = 1 second)
    startTimer(1000);
}

HelloWorldEditor::~HelloWorldEditor()
{
    stopTimer();
}

void HelloWorldEditor::paint(juce::Graphics& g)
{
    // Draw background gradient
    g.fillAll(juce::Colour(0xff1a1a2e));
    
    // Draw decorative border
    g.setColour(juce::Colours::cyan);
    g.drawRect(getLocalBounds(), 2);
    
    // Draw info text
    g.setColour(juce::Colours::lightgrey);
    g.setFont(12.0f);
    g.drawText("This proves JUCE GUI works inside FL Studio!", 
               10, getHeight() - 30, getWidth() - 20, 20,
               juce::Justification::centred);
}

void HelloWorldEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Title at top
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(20);  // Spacing
    
    // Timer label
    timerLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(20);  // Spacing
    
    // Gain control section
    auto gainSection = bounds.removeFromTop(40);
    gainLabel.setBounds(gainSection.removeFromLeft(60));
    gainValueLabel.setBounds(gainSection.removeFromRight(80));
    gainSlider.setBounds(gainSection.reduced(5, 0));
    bounds.removeFromTop(20);  // Spacing
    
    // Buttons in a row
    auto buttonRow = bounds.removeFromTop(40);
    auto buttonWidth = (buttonRow.getWidth() - 10) / 2;
    testButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(10);  // Gap between buttons
    fileChooserButton.setBounds(buttonRow);
    
    bounds.removeFromTop(20);  // Spacing
    
    // Status label
    statusLabel.setBounds(bounds.removeFromTop(30));
}

void HelloWorldEditor::timerCallback()
{
    // Update timer counter and label
    timerCounter++;
    timerLabel.setText("Timer: " + juce::String(timerCounter) + "s", 
                       juce::dontSendNotification);
}

void HelloWorldEditor::onTestButtonClicked()
{
    // Change status when button is clicked
    statusLabel.setText("Status: Button clicked at " + juce::String(timerCounter) + "s!", 
                        juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
}

void HelloWorldEditor::onFileChooserButtonClicked()
{
    // Create and show file chooser dialog
    auto chooser = std::make_shared<juce::FileChooser>(
        "Select a file to test dialog...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*"
    );
    
    auto flags = juce::FileBrowserComponent::openMode | 
                 juce::FileBrowserComponent::canSelectFiles;
    
    chooser->launchAsync(flags, [this, chooser](const juce::FileChooser& fc)
    {
        if (fc.getResult() != juce::File{})
        {
            auto fileName = fc.getResult().getFileName();
            statusLabel.setText("Status: Selected file - " + fileName, 
                              juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
        }
        else
        {
            statusLabel.setText("Status: File dialog cancelled", 
                              juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
        }
    });
}

void HelloWorldEditor::onGainSliderChanged()
{
    // Update gain value label
    double value = gainSlider.getValue();
    gainValueLabel.setText(juce::String(value, 2), juce::dontSendNotification);
    
    // Update processor parameter (convert to FL Studio range 0-65535)
    processor.setGainParameter(value);
    
    // Update status
    statusLabel.setText("Status: Gain changed to " + juce::String(value, 2), 
                        juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
}
