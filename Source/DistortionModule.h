/*
  ==============================================================================

    DistortionModule.h
    Created: 8 Apr 2024 11:04:36pm
    Author:  sasha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "adsr.h"
//==============================================================================
/*
*/
class DistortionModule  : public juce::Component
{
public:
    DistortionModule();
    ~DistortionModule() override;
    enum DistortionType {
        none,
        hardClip,
        softClip,
    };
    void setDistortionType(DistortionType);
    void setDistortionLevel(float);
    float getDistortionLevel();
	float apply(float sample) const;
private:
	float applyHardClip(float sample) const;
    float appySoftClip(float sample) const;

    float distLevel;
    DistortionType distType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionModule)
};
