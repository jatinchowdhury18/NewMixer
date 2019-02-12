#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor() : ProcessorBase (String ("Reverb Processor"))
{
    params.roomSize = 0.8f;
    params.wetLevel = 0.0f;
    params.dryLevel = 1.0f;
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

void ReverbProcessor::setDryWet (float wetAmt)
{
    params = reverbDsp.getParameters();
    params.wetLevel = wetAmt;
    params.dryLevel = 1.0f - wetAmt;
    reverbDsp.setParameters (params);
}
