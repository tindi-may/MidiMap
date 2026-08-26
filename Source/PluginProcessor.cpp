#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"

MidiMapAudioProcessor::MidiMapAudioProcessor() : parameters(*this, nullptr, "DC", Parameters::createParameterLayout()), updateRate(Parameters::defaultCCUpdateRate), modVelocity(Parameters::defaultModVelocity), ccNumberPan(Parameters::defaultCCNumberPan),
ccNumberBright(Parameters::defaultCCNumberBright), ccNumberRMS(Parameters::defaultCCNumberRMS), panIsActive(Parameters::defaultPanIsActive),
brightIsActive(Parameters::defaultBrightIsActive), rmsIsActive(Parameters::defaultRMSIsActive)
{
    Parameters::addListenerToAllParameters(parameters, this);
}

MidiMapAudioProcessor::~MidiMapAudioProcessor()
{
}

const juce::String MidiMapAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiMapAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiMapAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiMapAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiMapAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiMapAudioProcessor::getNumPrograms()
{
    return 1;
}
                

int MidiMapAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiMapAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MidiMapAudioProcessor::getProgramName (int index)
{
    return {};
}

void MidiMapAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void MidiMapAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    sampleCount = sr / updateRate;
    rms.prepareToPlay(sampleRate, samplesPerBlock);
    bright.prepareToPlay(sampleRate, samplesPerBlock);
}

void MidiMapAudioProcessor::releaseResources()
{
    rms.releaseResources();
    bright.releaseResources();
}

void MidiMapAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();

    if (auto* ed = dynamic_cast<MidiMapAudioProcessorEditor*>(getActiveEditor()))
    {
            ed->pushBufferToViewer(buffer);   
    }

    rms.processBlock(buffer);
    pan.processBlock(buffer);
    bright.processBlock(buffer);

    float dbVelocity = Decibels::gainToDecibels(pow(rms.getResult(numSamples), modVelocity), -48.0f);
    uint8 velocityValue = jlimit(1, 127, roundToInt(jmap(dbVelocity, -48.0f, 0.0f, 1.0f, 127.0f)));

    MidiBuffer auxMIDI;
    sampleCount -= numSamples;

    if (sampleCount <= 0)
    {
        if (panIsActive) {
            auto panCC = roundToInt(jlimit(0.0f, 127.0f,jmap(pan.getResult(), -1.0f, 1.0f, 0.0f, 127.0f)));
            auxMIDI.addEvent(MidiMessage::controllerEvent(1, ccNumberPan, panCC), 0);
        }

        if (brightIsActive) {
            auto brightCC = roundToInt(jlimit(0.0f, 127.0f, jmap(log(jlimit(20.0f, 20000.0f, bright.getResult())), log(20.0f), log(20000.0f), 0.0f, 127.0f)));
            auxMIDI.addEvent(MidiMessage::controllerEvent(1, ccNumberBright, brightCC), 0);
        }

        if (rmsIsActive) {
            float dbRms = Decibels::gainToDecibels(rms.getResult(numSamples), -48.0f);
            auto rmsValue = jlimit(1, 127, roundToInt(jmap(dbRms, -48.0f, 0.0f, 1.0f, 127.0f)));
            auxMIDI.addEvent(MidiMessage::controllerEvent(1, ccNumberRMS, rmsValue), 0);
        }

        sampleCount = sr / updateRate;
    }

    for (const auto midiMeta : midiMessages)
    {
        auto m = midiMeta.getMessage();
        const auto timeStamp = midiMeta.samplePosition;

        if (m.isNoteOn()) {
            m = juce::MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), velocityValue);
        }

        if (m.isController()) {
            int cn = m.getControllerNumber();
            if ((panIsActive && cn == ccNumberPan) ||
                (brightIsActive && cn == ccNumberBright) ||
                (rmsIsActive && cn == ccNumberRMS)) {
                continue;
            }
        }

        auxMIDI.addEvent(m, timeStamp);
    }

    midiMessages.swapWith(auxMIDI);
}

bool MidiMapAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MidiMapAudioProcessor::createEditor()
{
    return new MidiMapAudioProcessorEditor (*this, parameters);
}

void MidiMapAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState(); 
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MidiMapAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) { 
            parameters.replaceState(ValueTree::fromXml(*xmlState));
        }
    }
}

void MidiMapAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    if (paramID == Parameters::nameCCUpdateRate) {
        updateRate = newValue;
    }

    if (paramID == Parameters::nameModVelocity) {
        modVelocity = newValue;
    }

    if (paramID == Parameters::nameCCNumberPan) {
        ccNumberPan = newValue;
    }

    if (paramID == Parameters::nameCCNumberBright) {
        ccNumberBright = newValue;
    }

    if (paramID == Parameters::nameCCNumberRMS) {
        ccNumberRMS = newValue;
    }

    if (paramID == Parameters::namePanIsActive) {
        panIsActive = newValue;
    }

    if (paramID == Parameters::nameBrightIsActive) {
        brightIsActive = newValue;
    }

    if (paramID == Parameters::nameRMSIsActive) {
        rmsIsActive = newValue;
    }
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiMapAudioProcessor();
}
