/*
  ==============================================================================

    FilterModule.h
    Created: 13 Apr 2024 2:15:12pm
    Author:  sasha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ADSR.h"
//==============================================================================
/*
*/
class FilterModule {
public:
    FilterModule(double);

	float processSample(float sample);
    
    void setHighpass(bool *hp) {
        highpass = hp;
	}
    void setEnvLevel(float* env) {
		envLevel = env;
    }
    void setCutoff(float*);
    void setSampleRate(float sr);
    ADSRelem* adsr;
    float inline compA1(float, float);

private:
    float *envLevel;
    bool* highpass;
    float *cutoff;
    double sampleRate;
    float sampleBuffer = 0.f;
    float a1 = 0.f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
