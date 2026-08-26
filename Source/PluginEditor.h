#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginParameters.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class MidiMapAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::Timer 
{
public:
    MidiMapAudioProcessorEditor(MidiMapAudioProcessor&, AudioProcessorValueTreeState&);
    ~MidiMapAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void pushBufferToViewer(const juce::AudioBuffer<float>& bufferToPush);

private:
    void setupSlider(juce::Slider& slider, juce::String labelText);
    void setupToggle(juce::ToggleButton& toggle);

    MidiMapAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;

    juce::Slider rateSlider, velocitySlider, brightCCSlider, panCCSlider, rmsCCSlider;
    juce::ToggleButton panCCToggle, brightCCToggle, rmsCCToggle;


    std::unique_ptr<SliderAttachment> rateAttachment, velocityAttachment, brightCCSlAttachment, panCCSlAttachment, rmsCCSlAttachment;
    std::unique_ptr<ButtonAttachment> panCCBtnAttachment, brightCCBtnAttachment, rmsCCBtnAttachment;

    juce::AudioVisualiserComponent waveViewer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiMapAudioProcessorEditor)
};