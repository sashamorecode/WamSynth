/*
  ==============================================================================

    WavetableSynth.cpp
    Created: 5 Apr 2024 10:36:18pm
    Author:  sasha

  ==============================================================================
*/

#include "WavetableSynth.h"
void WavetableSynth::prepareToPlay(double sampleRate) {
    this->sampleRate = sampleRate;
    initOsc();
}

void WavetableSynth::initOsc() {
    this->curentOsc = 0;
    oscillators.clear();
    curWave = generateSineWaveTable();
    rerenderWavetable();
    for (auto i = 0; i < OSC_COUNT; ++i) {
        oscillators.emplace_back(&curWave, sampleRate);
    }
    
    for (auto& osc : oscillators) {
		osc.filter.setCutoff(&filterCutoff);
        osc.filter.setHighpass(&filterHighpass);
        osc.filter.setEnvLevel(&filterEnvLevel);
        osc.rescale();
    }
}


void WavetableSynth::setWavetable(const waveType type) {
    if (curWaveType == type) {
        return;
    }
    curWaveType = type;
    rerenderWavetable();
}

void WavetableSynth::rerenderWavetable() {
    switch (curWaveType)
    {
    case WavetableSynth::sine:
        curWave = generateSineWaveTable();
        break;
    case WavetableSynth::saw:
        curWave = generateSawWaveTable();
        break;
    case WavetableSynth::triangle:
        curWave = generateTriangleWaveTable();
        break;
    case WavetableSynth::rendered:
        curWave = generateRenderedWaveTable();
		break;
    default:
        break;
    }
}

void WavetableSynth::setADSR(float a, float d, float s, float r) {
    //if (oscillators[0].adsr.getADSR() == [a, d, s, r]);
    this->curADSRvals = ADSRelem::ADSRvals{ a,d,s,r };
}

void WavetableSynth::setTableLen(int l) {
    if (l == this->tableLen) { return; }
    this->tableLen = l;
    rerenderWavetable();
    for (auto& osc : oscillators) {
        osc.rescale();
    }
}


std::vector<float> WavetableSynth::generateSineWaveTable() {
    const float PI = juce::float_Pi;
    std::vector<float> sineWavetable = std::vector<float>(tableLen);
    for (auto i = 0; i < tableLen; ++i) {
        sineWavetable[i] = std::sinf(2 * PI * static_cast<float>(i) / tableLen);
    }
    return sineWavetable;
}

std::vector<float> WavetableSynth::generateSawWaveTable() {
    std::vector<float> sawWavetable = std::vector<float>(tableLen);
    for (auto i = 0; i < tableLen; ++i) {
        const float x = static_cast<float>(i)/tableLen;
        sawWavetable[i] = 2 *( x - std::floorf(x + 0.5));
    }
    return sawWavetable;
}

std::vector<float> WavetableSynth::generateTriangleWaveTable() {
    std::vector<float> triangleWavetable = std::vector<float>(tableLen);
    for (auto i = 0; i < tableLen; ++i) {
        const float x = 0.5f +  static_cast<float>(i) / tableLen;
        triangleWavetable[i] = 4.f * std::abs(fmod(x-0.25f, 1.f) - 0.5f) - 1.f;
    }
    return triangleWavetable;
}

std::vector<float> WavetableSynth::generateRenderedWaveTable() {
	std::vector<float> renderedWavetable = std::vector<float>(tableLen);
    curentOsc += 1;
    curentOsc = curentOsc % OSC_COUNT;
    float envLevel = 0.f;
    WavetableOsc osc = WavetableOsc(&curWave, sampleRate);
    osc.adsr.setADSR({0.f, 0.f, 1.f, 0.f});
	osc.filter.setCutoff(&filterCutoff);
    osc.filter.setHighpass(&filterHighpass);
    osc.filter.setEnvLevel(&envLevel);
    osc.setFreq(sampleRate / tableLen);
    osc.start(1.f);
    osc.rescale();
    for (auto i = 0; i < tableLen; ++i) {
		renderedWavetable[i] =  distortion.apply(osc.filter.processSample(osc.getSample()));
	}
    const float avg = std::accumulate(renderedWavetable.begin(), renderedWavetable.end(), 0.f) / tableLen;
    const float max = std::max_element(renderedWavetable.begin(), renderedWavetable.end())[0] - avg;
    for (auto i = 0; i < tableLen; ++i) {
		renderedWavetable[i] -= avg;
		renderedWavetable[i] /= max;
    }
    osc.stop();
    return renderedWavetable;
}

void WavetableSynth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer) {
    auto curSample = 0;
    //env + lfo section
    distortion.getDistortionLevel();

    for (const auto midiData : midiBuffer) {
        const auto message = midiData.getMessage();
        const auto messageTime = static_cast<int>(message.getTimeStamp());
        render(buffer, curSample, messageTime);
        curSample = messageTime;
        handleMidiEvent(message);
    }
    render(buffer, curSample, buffer.getNumSamples());
}

void WavetableSynth::render(juce::AudioBuffer<float>& buffer, int start, int end) {
    auto* firstChannel = buffer.getWritePointer(0);
    for (auto& osc : oscillators) {
        if (osc.isPlaying()) {
            for (auto sample = start; sample < end; ++sample) {
                const float env = osc.adsr.getSample();
				jassert(0.f <= env);
				jassert(1.f >= env);

                firstChannel[sample] += distortion.apply(osc.filter.processSample(osc.getSample()) * env ) * env * 0.5;
            }
        }
    }
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch) {
        auto* chData = buffer.getWritePointer(ch);
        std::copy(firstChannel + start, firstChannel + end, chData + start);
    }
}

void WavetableSynth::handleMidiEvent(const juce::MidiMessage& midiMessage) {
    if (midiMessage.isNoteOn()) {
        auto noteNum = midiMessage.getNoteNumber();
        auto noteVel = static_cast<float>(midiMessage.getVelocity())/127.f;
        while (oscillators[curentOsc].isPlaying()) {
            //this is to avoid note turning of if hold 1 note and play OSC_COUNT notes while holding
            //could crash if all oscs are playing
            curentOsc++;
            curentOsc = curentOsc % OSC_COUNT;
        }
        oscillators[curentOsc].adsr.setADSR(this->curADSRvals);
        const auto freq = midiNote2Freq(noteNum);
        oscillators[curentOsc].setFreq(freq);
        oscillators[curentOsc].start(noteVel);
        midi2id[noteNum] = curentOsc;
        curentOsc++;
        curentOsc = curentOsc % OSC_COUNT;
    }
    else if (midiMessage.isNoteOff()) {
        const auto noteNum = midiMessage.getNoteNumber();
        const auto oscId = midi2id[noteNum];
        midi2id[noteNum] = -1;
        if (oscId != -1) {
			oscillators[oscId].stop();
		}
    }
    else if (midiMessage.isAllNotesOff()) {
        for (auto& osc : oscillators) {
            osc.stop();
        }
    }
        
}

float WavetableSynth::midiNote2Freq(const int note) {
    return 440.f * std::powf(2.f, (static_cast<float>(note) - 69.f) / 12.f);
}
