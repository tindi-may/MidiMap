#pragma once
#include <JuceHeader.h>

namespace Parameters {

	static String getCCName(int cc) {
		switch (cc) {
		case 0:  return "Bank Select (MSB)";
		case 1:  return "Modulation";
		case 2:  return "Breath Controller";
		case 4:  return "Foot Controller";
		case 5:  return "Portamento Time";
		case 6:  return "Data Entry (MSB)";
		case 7:  return "Volume";
		case 8:  return "Balance";
		case 10: return "Pan";
		case 11: return "Expression";
		case 64: return "Damper Pedal";
		case 65: return "Portamento";
		case 66: return "Sostenuto";
		case 67: return "Soft Pedal";
		case 68: return "Legato Footswitch";
		case 71: return "Resonance (Filter)";
		case 72: return "Release Time";
		case 73: return "Attack Time";
		case 74: return "Frequency Cutoff";
		case 84: return "Portamento Control";
		case 91: return "Reverb Send Level";
		case 92: return "Tremolo Depth";
		case 93: return "Chorus Send Level";
		case 94: return "Detune Depth";
		case 95: return "Phaser Depth";
		default:
			if (cc >= 12 && cc <= 31) return "General Purpose";
			if (cc >= 32 && cc <= 63) return "LSB for CC 0-31";
			if (cc >= 102 && cc <= 119) return "Undefined/Custom";
			if (cc >= 120 && cc <= 127) return "Channel Mode Messages";
			return "Undefined";
		}
	}

	static const String nameCCUpdateRate = "UR";
	static const String nameModVelocity = "MV";
	static const String nameCCNumberPan = "CCP";
	static const String nameCCNumberBright = "CCB";
	static const String nameCCNumberRMS = "CCR";
	static const String namePanIsActive = "PAC";
	static const String nameBrightIsActive = "BAC";
	static const String nameRMSIsActive = "RAC";

	static const float defaultCCUpdateRate = 50.0f;
	static const float defaultModVelocity = 1.0f;
	static const int defaultCCNumberPan = 10;
	static const int defaultCCNumberBright = 74;
	static const int defaultCCNumberRMS = 11;
	static const bool defaultPanIsActive = true;
	static const bool defaultBrightIsActive = true;
	static const bool defaultRMSIsActive = true;

	static AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
		std::vector<std::unique_ptr<RangedAudioParameter>> params;

		int id = 1;

		auto intToString = [](int val, int maxLen) {
			return String(val) + ": " + getCCName(val);
			};

		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameCCUpdateRate, id++), "CC Update Rate (Hz)", 10.0f, 100.0f, defaultCCUpdateRate));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameModVelocity, id++), "Velocity", NormalisableRange<float>(0.25f, 4.0f, 0.01f, 0.5f), defaultModVelocity));

		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCCNumberPan, id++), "CC Number (Pan)", 0, 127, defaultCCNumberPan,
			AudioParameterIntAttributes().withStringFromValueFunction(intToString)));

		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCCNumberBright, id++), "CC Number (Brightness)", 0, 127, defaultCCNumberBright,
			AudioParameterIntAttributes().withStringFromValueFunction(intToString)));

		params.push_back(std::make_unique<AudioParameterInt>(ParameterID(nameCCNumberRMS, id++), "CC Number (RMS)", 0, 127, defaultCCNumberRMS,
			AudioParameterIntAttributes().withStringFromValueFunction(intToString)));

		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(namePanIsActive, id++), "Toggle CC (Pan)", defaultPanIsActive));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameBrightIsActive, id++), "Toggle CC (Bright)", defaultBrightIsActive));
		params.push_back(std::make_unique<AudioParameterBool>(ParameterID(nameRMSIsActive, id++), "Toggle CC (RMS)", defaultRMSIsActive));

		return { params.begin(), params.end() };
	}

	static void addListenerToAllParameters(AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* listener) {

		std::unique_ptr<XmlElement> xml(valueTreeState.copyState().createXml());

		for (auto* element : xml->getChildWithTagNameIterator("PARAM")) {
			const String& id = element->getStringAttribute("id"); 
			valueTreeState.addParameterListener(id, listener);
		}
	}
}