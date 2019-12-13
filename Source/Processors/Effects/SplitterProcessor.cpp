#include "SplitterProcessor.h"

SplitterProcessor::SplitterProcessor() :
    ProcessorBase ("Splitter")
{}

void SplitterProcessor::prepareToPlay (double sampleRate, int numSamples)
{
    setRateAndBufferSizeDetails (sampleRate, numSamples);
}

void SplitterProcessor::releaseResources()
{
}

void SplitterProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    for (int ch = 0; ch < 2; ++ch)
        buffer.copyFrom (ch + 2, 0, buffer.getReadPointer (ch), buffer.getNumSamples());

    if (wetGain == oldWetGain) // no fading needed
    {
        auto dryGain = 1.0f - wetGain;

        for (int ch = 0; ch < 2; ++ch)
        {
            buffer.applyGain (ch,     0, buffer.getNumSamples(), dryGain);
            buffer.applyGain (ch + 2, 0, buffer.getNumSamples(), wetGain);
        }
    }
    else
    {
        auto dryGain = 1.0f - wetGain;
        auto oldDryGain = 1.0f - oldWetGain;

        for (int ch = 0; ch < 2; ++ch)
        {
            buffer.applyGainRamp (ch,     0, buffer.getNumSamples(), oldDryGain, dryGain);
            buffer.applyGainRamp (ch + 2, 0, buffer.getNumSamples(), oldWetGain, wetGain);
        }

        oldWetGain = wetGain;
    }
}
