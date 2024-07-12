/*
  ==============================================================================

    DistortionModule.cpp
    Created: 8 Apr 2024 11:04:36pm
    Author:  sasha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DistortionModule.h"

//==============================================================================
DistortionModule::DistortionModule()
    :distLevel{0.f},
    distType{DistortionType::none}

{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

DistortionModule::~DistortionModule()
{
}

void DistortionModule::setDistortionType(DistortionType dtype) {
    this->distType = dtype;
}

void DistortionModule::setDistortionLevel(float level) {
    this->distLevel = level;
}

float DistortionModule::getDistortionLevel() {
    return distLevel;
}

float DistortionModule::applyHardClip(float sample) const{
    
    sample *= (1.f + distLevel * 8.f);
    if (sample > 1.f) {
        return 1.f;
    }
    if (sample < -1.f) {
        return -1.f;
    }
    return sample;
}

float DistortionModule::appySoftClip(float inputSample) const {
    const auto sample = inputSample * (0.5f + distLevel * 10.f);
    if (sample > 0.57735) {
        return 1.f;
    }
    if (sample < -0.57735) {
        return -1.f;
    }
    return (sample - powf(sample, 3.f) ) * (1.f/0.3849f); //min max of function to 1'
}

float DistortionModule::apply(float sample) const {
    switch (distType)
    {
    case DistortionModule::none:
        return sample;
    case DistortionModule::hardClip:
        return applyHardClip(sample);
        break;
    case DistortionModule::softClip:
        return appySoftClip(sample);
        break;
    default:
        return sample;
        break;
    }
}