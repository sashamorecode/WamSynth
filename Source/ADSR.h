/*
  ==============================================================================

    ADSR.h
    Created: 6 Apr 2024 6:48:21pm
    Author:  sasha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ADSRelem
{
public:
    struct ADSRvals {
        float attack;
        float decay;
        float sustain;
        float relese;
    };
    ADSRelem(double sampleRate);
    ADSRelem(const ADSRelem&) = delete;
    ADSRelem& operator = (const ADSRelem&) = delete;
    ADSRelem(ADSRelem&&) = default;
    ADSRelem& operator = (ADSRelem&&) = default;

    void setADSR(float, float, float, float);
    void setADSR(ADSRvals);
    float getSample();
    void const noteOn();
    void const noteOff();
    bool isPlaying() const;
    
    float getEnvVal() const;

private:
    ADSRvals adsrVals;
    double sampleRate;
    float time;
    float releseTime;
    bool keyDown;
    float ampAtKeyDown;

};
