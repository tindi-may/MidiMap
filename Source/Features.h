#pragma once
#include <JuceHeader.h>

class RRMS {
public:
	RRMS(float defaultTime = 0.035f, float defaultMaxTime = 1.0f)
		: rmsTime(defaultTime), maxTime(defaultMaxTime) {}
	~RRMS() {}

	void prepareToPlay(double sr, int samplesPerBlock);

	void releaseResources();

	void processBlock(AudioBuffer<float>& buffer);

	float getResult(const int numSamples) const;


private:
	void reset();

	AudioBuffer<float> aux; 
	
	float avg = 0;
	AudioBuffer<float> history; 
	int historyIndex = 0;
	float maxTime; 
	double sampleRate;
	float rmsTime;
	int windowSize; 

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RRMS)
};

class PAN {
public:
	PAN() {}

	~PAN() {}

	void processBlock(AudioBuffer<float>& buffer);

	float getResult() const { return panValue; }

private: 
	float panValue = 0.0f;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PAN);
};

class Brightness {
public:
	Brightness() {}

	~Brightness() {}

	void prepareToPlay(double sr, int samplesPerBlock);

	void releaseResources();

	void processBlock(AudioBuffer<float>& buffer);

	float getResult() const { return brightValue; }

private:
	AudioBuffer<float> filterBuffer;
	float brightValue = 0.0f;
	double sampleRate;
	RRMS envelope, filterEnvelope;
	float lastSample;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Brightness);
};
