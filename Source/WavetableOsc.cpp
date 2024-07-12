/*
  ==============================================================================

    WavetableOsc.cpp
    Created: 5 Apr 2024 10:48:03pm
    Author:  sasha

  ==============================================================================
*/
#include <cmath>
#include "JuceHeader.h"
#include "WavetableOsc.h"

WavetableOsc::WavetableOsc(std::vector<float> *waveTable, double sampleRate)
    :waveTable{waveTable},
    sampleRate{sampleRate},
    adsr{ sampleRate },
    curFreq{0.f},
    tableLen{64},
    filter{sampleRate}
{
    filter.adsr = &adsr;
}

WavetableOsc::WavetableOsc(const WavetableOsc& w)
    :waveTable{ w.waveTable },
    sampleRate{ w.sampleRate },
    adsr{ w.sampleRate },
    curFreq{ w.curFreq },
    tableLen{ w.tableLen },
    filter{ w.sampleRate }
{
	filter.adsr = &adsr;
    rescale();
}

void WavetableOsc::rescale() {
    index = index * (static_cast<float>(waveTable->size())/static_cast<float>(tableLen) );
    tableLen = waveTable->size();
    reComputeIndexInc();
}

float WavetableOsc::getSample() {
    index = std::fmod(index, static_cast<float>(tableLen));
    jassert(index >= 0);
    const auto sample = interpolateLin();
    index += indexInc;
    return sample * velocity;
}

float WavetableOsc::interpolateLin() const {
    const auto idxFloor = static_cast<std::vector<float>::size_type>(index);
    const auto idxCeil = static_cast<std::vector<float>::size_type>
                                            (std::ceil(index)) % tableLen;
    
    const auto idxCeilWeight = index - static_cast<float>(idxFloor);
    return (*waveTable)[idxCeil] * idxCeilWeight + (*waveTable)[idxFloor] * (1.f - idxCeilWeight);
}


void WavetableOsc::setFreq(float freq){
    curFreq = freq;
    reComputeIndexInc();
    }

void WavetableOsc::reComputeIndexInc() {
    indexInc = curFreq * static_cast<float>(tableLen)
        / static_cast<float>(sampleRate);
}

void WavetableOsc::stop() {
    adsr.noteOff();
}
void WavetableOsc::start(float vel) {
    velocity = vel;
    rescale();
    adsr.noteOn();
}

bool WavetableOsc::isPlaying() const {
    return adsr.isPlaying();
}

