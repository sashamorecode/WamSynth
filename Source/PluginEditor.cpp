/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    const auto textJust = juce::Justification::topRight;
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "attack", attackSlider));
    addAndMakeVisible(attackLabel);
    attackLabel.setText("a", juce::dontSendNotification);
    attackLabel.setJustificationType(textJust);

    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    decayAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "decay", decaySlider));
    addAndMakeVisible(decayLabel);
    decayLabel.setText("d", juce::dontSendNotification);
    decayLabel.setJustificationType(textJust);

    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sustainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "sustain", sustainSlider));
    addAndMakeVisible(sustainLabel);
	sustainLabel.setText("s", juce::dontSendNotification);
    sustainLabel.setJustificationType(textJust);

    addAndMakeVisible(releseSlider);
    releseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    releseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "relese", releseSlider));
    addAndMakeVisible(releseLabel);
    releseLabel.setText("r", juce::dontSendNotification);
	releseLabel.setJustificationType(textJust);
    
    addAndMakeVisible(wavetable);
    wavetable.addItem("Sin", 1);
    wavetable.addItem("Saw", 2);
    wavetable.addItem("Triangle", 3);
    wavetable.addItem("ReRendr", 4);
    wavetableAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(vts, "wavetable", wavetable));
    
    addAndMakeVisible(reRenderWavetableButton);
    reRenderWavetableButton.setButtonText("ReRender");
    reRenderWavetableButton.setToggleable(false);
    reRenderWavetableAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(vts, "re_render_wave", reRenderWavetableButton));

    addAndMakeVisible(wavetableResolutionSlider);
    wavetableResolutionSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    wavetableResolutionAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "wavetable_resolution", wavetableResolutionSlider));
    addAndMakeVisible(wavetableResolutionLabel);
    wavetableResolutionLabel.setText("Resolution", juce::dontSendNotification);
    wavetableResolutionLabel.setJustificationType(textJust);

    addAndMakeVisible(distortionButton);
    distortionButton.setButtonText("Distortion");
    distortionButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(vts, "distortion_toggle", distortionButton));

    addAndMakeVisible(distortionType);
    distortionType.addItem("none", 1);
    distortionType.addItem("Hard clip",2);
    distortionType.addItem("Soft clip", 3);
    distortionTypeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(vts, "distortion_type", distortionType));

    addAndMakeVisible(distortionLevel);
    distortionLevel.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    distoritonLevelAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "distortion_level", distortionLevel));
    addAndMakeVisible(distortionLabel);
    distortionLabel.setText("Distortion Level", juce::dontSendNotification);
    distortionLabel.setJustificationType(textJust);
    
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    filterCutoffAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "filter_cutoff", filterCutoffSlider));
    addAndMakeVisible(filterCutoffLabel);
    filterCutoffLabel.setText("Cutoff Freq", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType(textJust);

    addAndMakeVisible(filterEnvSlider);
    filterEnvSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    filterEnvAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "filter_env", filterEnvSlider));
    addAndMakeVisible(filterEnvLabel);
    filterEnvLabel.setText("Env->Filter", juce::dontSendNotification);
	filterEnvLabel.setJustificationType(textJust);

    addAndMakeVisible(filterButton);
    filterButton.setButtonText("High/Low Toggle");
    filterButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(vts, "filter_toggle", filterButton));
    
    setSize (570, 320);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    juce::Image background = juce::ImageFileFormat::loadFrom(BinaryData::backgoundImg_png, BinaryData::backgoundImg_pngSize);
    g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const int margin = 10;
    const int sliderH = 120;
    const int sliderW = 80;
    const int labelH = 20;
    const int labelW = 80;
    attackSlider.setBounds({ margin, margin, sliderW, sliderH });
    attackLabel.setBounds({ margin, attackSlider.getBottom(), labelW, labelH });
    decaySlider.setBounds({ attackSlider.getRight() + margin, margin, sliderW, sliderH });
    decayLabel.setBounds({ decaySlider.getX(), decaySlider.getBottom(), labelW, labelH });
    sustainSlider.setBounds({ decaySlider.getRight() + margin, margin, sliderW, sliderH });
    sustainLabel.setBounds({ sustainSlider.getX(), sustainSlider.getBottom(), labelW, labelH });
    releseSlider.setBounds({ sustainSlider.getRight() + margin, margin, sliderW, sliderH });
    releseLabel.setBounds({ releseSlider.getX(), releseSlider.getBottom(), labelW, labelH });
    wavetable.setBounds({ releseSlider.getRight() + margin, margin,sliderW,sliderH/4 });
    reRenderWavetableButton.setBounds({wavetable.getX(), wavetable.getBottom(), 100, 50});

    wavetableResolutionSlider.setBounds({reRenderWavetableButton.getRight() + margin, margin, sliderW, sliderH});
    wavetableResolutionLabel.setBounds({wavetableResolutionSlider.getX(), wavetableResolutionSlider.getBottom(), labelW, labelH});

    const int row2H = attackLabel.getBottom();
    distortionButton.setBounds({ margin, row2H + margin, 100, 50 });
    distortionType.setBounds({ margin, distortionButton.getBottom() + margin, sliderW, sliderH/4});
    distortionLevel.setBounds({ distortionType.getRight() + margin, row2H + margin, sliderW *2, sliderH });
    distortionLabel.setBounds({ distortionLevel.getX(), distortionLevel.getBottom(), labelW, labelH });
    filterCutoffSlider.setBounds({ distortionLevel.getRight() + margin, row2H+ margin, sliderW, sliderH });
    filterCutoffLabel.setBounds({ filterCutoffSlider.getX(), filterCutoffSlider.getBottom(), labelW, labelH });
    filterEnvSlider.setBounds({ filterCutoffSlider.getRight() + margin, row2H + margin, sliderW, sliderH });
    filterEnvLabel.setBounds({ filterEnvSlider.getX(), filterEnvSlider.getBottom(), labelW, labelH});
    filterButton.setBounds({ filterEnvSlider.getRight() + margin, row2H + filterEnvSlider.getHeight()/2, sliderW,sliderH / 4});
}
