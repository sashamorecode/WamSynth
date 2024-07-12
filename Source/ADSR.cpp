/*
  ==============================================================================

    ADSR.cpp
    Created: 6 Apr 2024 6:48:21pm
    Author:  sasha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSR.h"

//==============================================================================

ADSRelem::ADSRelem(double sampleRate) : sampleRate{ sampleRate }, adsrVals{ 0.1f, 0.1f,0.7f,0.1f },
time{ 0.f }, releseTime{ 100.f }, ampAtKeyDown{ 0.f }, keyDown{ false } {}

void ADSRelem::setADSR(float a, float d, float s, float r) {
    this->adsrVals = ADSRvals{ a,d,s,r };
}

void ADSRelem::setADSR(ADSRvals adsr) {
    this->adsrVals = adsr;
}

const void ADSRelem::noteOn() {
    this->time = 0.f;
    this->releseTime = 0.f;
    this->keyDown = true;
}

const void ADSRelem::noteOff() {
    this->ampAtKeyDown = getEnvVal();
    this->releseTime = 0.f;
    this->keyDown = false;
}

bool ADSRelem::isPlaying() const {
    if (keyDown) { return true; }
    return (getEnvVal() != 0.f);
}

float ADSRelem::getSample() {
    if (releseTime > adsrVals.relese && !keyDown) { return 0.f; }
    const auto envVal = getEnvVal();
    if (keyDown) {
        time += 1 / sampleRate;
    }
    else {
        releseTime += 1 / sampleRate;
    }
    jassert(releseTime >= 0.f);
    jassert(time >= 0.f);
    return envVal;
}

float ADSRelem::getEnvVal() const{
	if (!keyDown) {
        if (adsrVals.relese == 0) { return 0; }
        const float amp = (1.f - releseTime / adsrVals.relese) * ampAtKeyDown;
        if (amp < 0.f) { return 0.f; }
        return amp;
    }
    else if (time <= adsrVals.attack) {
        if (adsrVals.attack == 0.f) {
            return 1.f;
        }
        return (time/adsrVals.attack);
    }
    else if (time <= adsrVals.attack + adsrVals.decay) {
        const float timeDecay = time - adsrVals.attack;
        
        return (1.f - timeDecay/adsrVals.decay) * (1.f - adsrVals.sustain) + adsrVals.sustain;
    }
    else {
        return adsrVals.sustain;
    }

}
