/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    juce::Slider attackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    juce::Label sustainLabel;

    juce::Slider releseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releseAttachment;
	juce::Label releseLabel;

    juce::ComboBox wavetable;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> wavetableAttachment;
    
    juce::Slider wavetableResolutionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wavetableResolutionAttachment;
    juce::Label wavetableResolutionLabel;

    juce::ToggleButton reRenderWavetableButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reRenderWavetableAttachment;

    juce::Slider wavetableEnvSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wavetableEnvAttachment;
    juce::Label wavetableEnvLabel;

    juce::ToggleButton distortionButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> distortionButtonAttachment;

    juce::Slider distortionLevel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distoritonLevelAttachment;
    juce::Label distortionLabel;

    juce::ComboBox distortionType;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionTypeAttachment;
    
    juce::Slider filterCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
	juce::Label filterCutoffLabel;

    juce::ToggleButton filterButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> filterButtonAttachment;

    juce::Slider filterEnvSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterEnvAttachment;
	juce::Label filterEnvLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
