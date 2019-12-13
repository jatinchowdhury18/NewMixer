#include "ReverbProcessor.h"

namespace
{
    const String sizeID  = "size";
    const String dampID  = "damp";
    const String widthID = "width";
    const String dwID    = "drywet";
}

ReverbProcessor::ReverbProcessor() :
    vts (*this, nullptr, Identifier ("Parameters"), 
            AudioProcessorValueTreeState::ParameterLayout
            (
                std::make_unique<AudioParameterFloat> (sizeID,  "Size",    0.0f, 1.0f, 0.5f),
                std::make_unique<AudioParameterFloat> (dampID,  "Damping", 0.0f, 1.0f, 0.5f),
                std::make_unique<AudioParameterFloat> (widthID, "Width",   0.0f, 1.0f, 1.0f),
                std::make_unique<AudioParameterFloat> (dwID,    "Dry/Wet", 0.0f, 1.0f, 1.0f)
            )
        )
{
    setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    sizeParam  = vts.getRawParameterValue (sizeID);
    dampParam  = vts.getRawParameterValue (dampID);
    widthParam = vts.getRawParameterValue (widthID);
    dwParam    = vts.getRawParameterValue (dwID);

    setParameters();
}

void ReverbProcessor::fillInPluginDescription (PluginDescription& desc) const
{
    desc.name = "Reverb";
    desc.descriptiveName = "NewMixer Internal Reverb";
    desc.category = "Reverbs";
    desc.manufacturerName = "chowdsp";
    desc.version = String (JUCEApplication::getInstance()->getApplicationVersion());
    desc.numInputChannels = 2;
    desc.numOutputChannels = 2;
}

void ReverbProcessor::prepareToPlay (double sampleRate, int maxExpectedBlockSize)
{
    dsp::ProcessSpec spec;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = maxExpectedBlockSize;

    reverbDsp.prepare (spec);
    starting = true;
}

void ReverbProcessor::releaseResources()
{
    reverbDsp.reset();
}

void ReverbProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiBuffer*/)
{
    if (starting)
    {
        starting = false;
        return;
    }

    setParameters();

    auto block = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (block);
    reverbDsp.process (context);
}

void ReverbProcessor::setParameters()
{
    params.roomSize = *sizeParam;
    params.damping = *dampParam;
    params.width = *widthParam;
    params.wetLevel = *dwParam;
    params.dryLevel = 1.0f - *dwParam;
    params.freezeMode = 0.0f;

    reverbDsp.setParameters (params);
}
