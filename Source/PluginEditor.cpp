#include "PluginProcessor.h"
#include "PluginEditor.h"

MidiMapAudioProcessorEditor::MidiMapAudioProcessorEditor(MidiMapAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts), waveViewer(2)
{
    waveViewer.setOpaque(false);
    waveViewer.setRepaintRate(30);
    waveViewer.setBufferSize(1024);
    waveViewer.setSamplesPerBlock(256); 
    waveViewer.setColours(juce::Colours::transparentBlack, juce::Colours::lightgreen);
    addAndMakeVisible(waveViewer);

    rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
    addAndMakeVisible(rateSlider);

    velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    velocitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
    addAndMakeVisible(velocitySlider);

    setupSlider(panCCSlider, "Pan CC #");
    setupSlider(brightCCSlider, "Brightness CC #");
    setupSlider(rmsCCSlider, "RMS CC #");
    setupToggle(panCCToggle);
    setupToggle(brightCCToggle);
    setupToggle(rmsCCToggle);

    rateAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameCCUpdateRate, rateSlider));
    velocityAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameModVelocity, velocitySlider));
    panCCSlAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameCCNumberPan, panCCSlider));
    brightCCSlAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameCCNumberBright, brightCCSlider));
    rmsCCSlAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameCCNumberRMS, rmsCCSlider));

    panCCBtnAttachment.reset(new ButtonAttachment(valueTreeState, Parameters::namePanIsActive, panCCToggle));
    brightCCBtnAttachment.reset(new ButtonAttachment(valueTreeState, Parameters::nameBrightIsActive, brightCCToggle));
    rmsCCBtnAttachment.reset(new ButtonAttachment(valueTreeState, Parameters::nameRMSIsActive, rmsCCToggle));

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::lightgreen);
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightgreen);
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::ToggleButton::tickColourId, juce::Colours::lightgreen);

    setSize(800, 500);
    startTimer(30);
}

MidiMapAudioProcessorEditor::~MidiMapAudioProcessorEditor() { stopTimer(); }

void MidiMapAudioProcessorEditor::pushBufferToViewer(const juce::AudioBuffer<float>& bufferToPush)
{
    waveViewer.pushBuffer(bufferToPush);
}

void MidiMapAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(juce::Colours::rebeccapurple, 0, 0,
        juce::Colours::seagreen, 0, (float)getHeight(), false));
    g.fillAll();

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawVerticalLine(300, 20.0f, (float)getHeight() - 20.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText("CC Rate", 25, 25, 120, 20, juce::Justification::centred);
    g.drawText("Velocity Mod", 155, 25, 120, 20, juce::Justification::centred);

    auto drawCCLabel = [&](juce::String text, int y) {
        g.drawText(text, 65, y, 210, 20, juce::Justification::centred);
        };
    drawCCLabel("CC Number (Pan)", 160);
    drawCCLabel("CC Number (Brightness)", 270);
    drawCCLabel("CC Number (RMS)", 380);

    juce::Rectangle<float> waveFrame(320.0f, 30.0f, 460.0f, 220.0f);
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillRoundedRectangle(waveFrame, 10.0f);
    g.setColour(juce::Colours::lightgreen.withAlpha(0.5f));
    g.drawRoundedRectangle(waveFrame, 10.0f, 2.0f);

    juce::Rectangle<int> logoArea(320, 280, 460, 180);
    g.setColour(juce::Colours::rebeccapurple.withAlpha(0.3f));
    g.fillRoundedRectangle(logoArea.toFloat(), 15.0f);

    g.setColour(juce::Colours::lightgreen.withAlpha(0.9f));
    g.setFont(juce::FontOptions(60.0f).withStyle("Bold Italic"));
    g.drawText("MIDI MAP", 320, 315, 460, 70, juce::Justification::centred);

    g.setColour(juce::Colours::white.withAlpha(0.55f));
    g.setFont(juce::FontOptions(16.0f).withKerningFactor(0.18f));
    g.drawText("MIDI DYNAMICS CONTROLLER", 320, 385, 460, 30, juce::Justification::centred);

    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.setFont(juce::FontOptions(12.0f).withStyle("Italic"));
    g.drawText("Coded by Alice May Tindiglia @ LIM :: V1.0", 320, 470, 460, 20, juce::Justification::centred);
}

void MidiMapAudioProcessorEditor::resized()
{
    rateSlider.setBounds(25, 50, 120, 100);
    velocitySlider.setBounds(155, 50, 120, 100);

    auto setupCCRow = [&](juce::ToggleButton& t, juce::Slider& s, int y) {
        t.setBounds(20, y + 18, 40, 40);
        s.setBounds(65, y + 25, 210, 45);
        };

    setupCCRow(panCCToggle, panCCSlider, 160);
    setupCCRow(brightCCToggle, brightCCSlider, 270);
    setupCCRow(rmsCCToggle, rmsCCSlider, 380);

    waveViewer.setBounds(328, 38, 444, 204);
}

void MidiMapAudioProcessorEditor::timerCallback()
{
    auto updateSliderStyle = [](juce::Slider& s, bool enabled) {
        s.setEnabled(enabled);

        if (enabled)
        {
            s.setColour(juce::Slider::thumbColourId, juce::Colours::lightgreen);
            s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightgreen);
            s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        }
        else
        {
            s.setColour(juce::Slider::thumbColourId, juce::Colours::grey);
            s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::grey);
            s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        }
        };

    updateSliderStyle(panCCSlider, panCCToggle.getToggleState());
    updateSliderStyle(brightCCSlider, brightCCToggle.getToggleState());
    updateSliderStyle(rmsCCSlider, rmsCCToggle.getToggleState());
}

void MidiMapAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::String labelText)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 150, 16);
    slider.setChangeNotificationOnlyOnRelease(true);
    addAndMakeVisible(&slider);
}

void MidiMapAudioProcessorEditor::setupToggle(juce::ToggleButton& toggle)
{
    addAndMakeVisible(&toggle);
}