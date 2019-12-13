#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor() : ProcessorBase (String ("Reverb Processor"))
{
    setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    dsp::Reverb::Parameters params;
    params.roomSize = 0.8f;
    params.wetLevel = 1.0f;
    params.dryLevel = 0.0f;
    reverbDsp.setParameters (params);
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

    auto block = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (block);
    reverbDsp.process (context);
}

void ReverbProcessor::setParameters (dsp::Reverb::Parameters& params)
{
    reverbDsp.setParameters (params);
}
