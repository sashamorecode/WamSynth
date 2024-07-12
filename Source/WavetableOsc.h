/*
  ==============================================================================

    WavetableOsc.h
    Created: 5 Apr 2024 10:48:03pm
    Author:  sasha

  ==============================================================================
*/

#pragma once

#include <vector>
#include "ADSR.h"
#include "FilterModule.h"

class WavetableOsc {
public:
    WavetableOsc(std::vector<float> *waveTable, double sampleRate);
    WavetableOsc(const WavetableOsc& w);
    WavetableOsc& operator = (const WavetableOsc&) = delete;
    WavetableOsc(WavetableOsc&&) = default;
    WavetableOsc& operator = (WavetableOsc&&) = default;

    float getSample();
    void setFreq(float);
    void stop();
    void start(float);
    bool isPlaying() const;
	ADSRelem adsr;
    void rescale();
    void reComputeIndexInc();

    FilterModule filter;
private:
    float interpolateLin() const;
    float index = 0.f;
    float indexInc = 0.f;
    std::vector<float>* waveTable;
    float velocity = 0.f;
    int tableLen;
    double sampleRate;
    float curFreq;
    };
