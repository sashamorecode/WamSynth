/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#else
#endif
parameters(*this, nullptr, juce::Identifier("WamSynth"),
    {
        std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange{0.f,2.f, 0.0001f, 0.7f}, 0.2f),
        std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange{0.f,2.f, 0.0001f, 0.7f}, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.f, 1.f, 0.65f),
        std::make_unique<juce::AudioParameterFloat>("relese", "Relese",juce::NormalisableRange{0.f,2.f, 0.0001f, 0.7f}, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("wavetable", "Wavetable", 0, 3, 0),
        std::make_unique<juce::AudioParameterBool>("distortion_toggle", "Distortion Toggle",false),
        std::make_unique<juce::AudioParameterFloat>("distortion_type", "Distorion Type", 0, 2, 1),
        std::make_unique<juce::AudioParameterFloat>("distortion_level", "Distortion Level", juce::NormalisableRange{0.f,1.f, 0.001f,0.4f,false},0.f),
        std::make_unique<juce::AudioParameterFloat>("wavetable_resolution", "Wavetable Resolution", juce::NormalisableRange{3.f, 1024.f, 1.f, 0.35f, false}, 64.f),
        std::make_unique<juce::AudioParameterBool>("re_render_wave", "Rerender Wave", false),
        std::make_unique<juce::AudioParameterFloat>("filter_cutoff", "Filter Cutoff", juce::NormalisableRange{20.f,20000.f, 0.001f, 0.5f}, 440.f),
        std::make_unique<juce::AudioParameterBool>("filter_toggle", "Filter Toggle", false),
        std::make_unique<juce::AudioParameterFloat>("filter_env", "Filter Env", -1000.f, 1000.f, 0.5f),
        }
    )
{
    attack = parameters.getRawParameterValue("attack");
    decay = parameters.getRawParameterValue("decay");
    sustain = parameters.getRawParameterValue("sustain");
    relese = parameters.getRawParameterValue("relese");
    wavetableType = parameters.getRawParameterValue("wavetable");
    distortionToggle = parameters.getRawParameterValue("distortion_toggle");
    distortionTypeFloat = parameters.getRawParameterValue("distortion_type");
    distortionLevel = parameters.getRawParameterValue("distortion_level");
    wavetableResolution = parameters.getRawParameterValue("wavetable_resolution");
    reRender = parameters.getRawParameterValue("re_render_wave");

    filterCutoff = parameters.getRawParameterValue("filter_cutoff");
    filterHighpass = parameters.getRawParameterValue("filter_toggle");
    filterEnvLevel = parameters.getRawParameterValue("filter_env");


}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.prepareToPlay(sampleRate);
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    int tableLen = wavetableResolution->load();
    synth.setTableLen(tableLen);

    synth.setADSR(attack->load(), decay->load(), sustain->load(), relese->load());
    synth.filterCutoff = filterCutoff->load();
    synth.filterHighpass = filterHighpass->load() > 0.5 ? true : false;
    synth.filterEnvLevel = filterEnvLevel->load();
	const bool distToggleBool = distortionToggle->load() > 0.5 ? 1.f : 0.f;
    if (!distToggleBool) {
        synth.distortion.setDistortionType(DistortionModule::none);
    }
    else {
        
        synth.distortion.setDistortionType(static_cast<DistortionModule::DistortionType>(distortionTypeFloat->load()));
        synth.distortion.setDistortionLevel(distortionLevel->load());
    }

    if (reRender->load()) {
        //wavetableType->store(WavetableSynth::rendered);
        parameters.getParameter("wavetable")->setValueNotifyingHost(static_cast<float>(WavetableSynth::rendered));
        parameters.getParameter("re_render_wave")->setValueNotifyingHost(0.f);
		synth.rerenderWavetable();
	}

    const float wavetableTypeParam = wavetableType->load();
    const WavetableSynth::waveType waveType = static_cast<WavetableSynth::waveType>(wavetableTypeParam);
   	synth.setWavetable(waveType);
	synth.processBlock(buffer, midiMessages);

}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(parameters.state.createXml());
    
    const std::vector<float> wavetable = synth.getCurWave();
    MemoryBlock wavetableData;
    {
        auto* waveOut = new MemoryOutputStream(wavetableData, true);
        waveOut->write(wavetable.data(), wavetable.size() * sizeof(float));
        waveOut->flush();
    }
    xml->setAttribute("wavetable_data", wavetableData.toBase64Encoding());
    copyXmlToBinary(*xml, destData);
    xml.release();
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr) {
        if (xml->hasTagName(parameters.state.getType())) {
            
			juce::ValueTree newState = juce::ValueTree::fromXml(*xml);
            parameters.replaceState(newState);
		}
	}
    const auto waveAttr = parameters.getRawParameterValue("wavetable")->load();
    const auto waveType = static_cast<WavetableSynth::waveType>(waveAttr);
    if(xml.get() != nullptr){
		const juce::StringRef wavetableData = xml->getStringAttribute("wavetable_data");
		if (wavetableData.isNotEmpty() && waveType == WavetableSynth::rendered) {
			MemoryBlock waveMem;
			waveMem.fromBase64Encoding(wavetableData);
			auto* waveIn = new MemoryInputStream(waveMem, true);
			std::vector<float> wavetable;
			wavetable.resize(waveMem.getSize() / sizeof(float));
			waveIn->read(wavetable.data(), waveMem.getSize());
			synth.setWaveTableData(wavetable);
			synth.setTableLen(wavetable.size());
		}
	}
    synth.prepareToPlay(getSampleRate());
    xml.release();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
