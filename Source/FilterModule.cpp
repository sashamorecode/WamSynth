/*
  ==============================================================================

    FilterModule.cpp
    Created: 13 Apr 2024 2:15:12pm
    Author:  sasha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterModule.h"

//==============================================================================
FilterModule::FilterModule(double sr)
    : sampleRate{ sr }
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}


float inline FilterModule::compA1(float cutoff, float sr) {
    const auto tan = std::tan(juce::float_Pi * std::fmax((cutoff + adsr->getEnvVal() * (*envLevel)), 1.f) / sr);
    return (tan - 1.f) / (tan + 1.f);
}

void FilterModule::setCutoff(float* cuoff) {
	cutoff = cuoff;
}


float FilterModule::processSample(float sample) {
    a1 = compA1(*cutoff, sampleRate);
    const float sign = (*highpass) ? -1.f : 1.f;
    const auto allPass = a1 * sample + sampleBuffer;
    sampleBuffer = sample - a1 * allPass;
    return 0.5f * (sign * allPass + sample);
}
