/*
  ==============================================================================

    WavetableSynth.h
    Created: 5 Apr 2024 10:36:18pm
    Author:  sasha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOsc.h"
#include "DistortionModule.h"

#define OSC_COUNT 512
#define MAX_TABLE_LEN 512
class WavetableSynth {
public:
    void prepareToPlay(double sampleRate);
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void setADSR(float, float, float, float);
    void setTableLen(int);
    enum waveType {sine, saw, triangle, rendered};
    void setWavetable(const waveType);
    DistortionModule distortion;
    float distortionEnvLevel;
    int tableLen = 64;

    float filterCutoff = 0.5f;
    bool filterHighpass = false;
    float filterEnvLevel = 0.f;

    std::vector<float> getCurWave(){ return curWave; }
    void setWaveTableData(const std::vector<float>& data) {
        curWave = data;
        rerenderWavetable();
    }
    void rerenderWavetable();
private:
    std::vector<float> generateSineWaveTable(); 
	std::vector<float> generateSawWaveTable();
    std::vector<float> generateTriangleWaveTable();
    std::vector<float> generateRenderedWaveTable();
    static float midiNote2Freq(int);
    void initOsc();
    void handleMidiEvent(const juce::MidiMessage&);
    void render(juce::AudioBuffer<float>& buff, int beginSmp, int endSmp);
    double sampleRate;
    int samplesPerBlock;
    int curentOsc;
    std::vector<WavetableOsc> oscillators;
    int midi2id[OSC_COUNT] = {};
    ADSRelem::ADSRvals curADSRvals;
    std::vector<float> curWave;
    waveType curWaveType;
};
