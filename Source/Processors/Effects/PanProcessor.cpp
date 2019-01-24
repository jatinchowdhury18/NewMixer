#include "PanProcessor.h"

PanProcessor::PanProcessor() : ProcessorBase (String ("Pan Processor"))
{
    updateChannelGains();
}

void PanProcessor::setPan (float newPan)
{
    pan = newPan;
    updateChannelGains();
}

void PanProcessor::updateChannelGains()
{
    curLeftGain = cos (pan * halfPi);
    curRightGain = sin (pan * halfPi);
}

void PanProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);
    updateChannelGains();
}

void PanProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    if (! buffer.hasBeenCleared() && buffer.getNumChannels() >= 2)
    {
        const int numSamples = buffer.getNumSamples();

        if (curLeftGain != oldLeftGain)
        {
            buffer.applyGainRamp (0, 0, numSamples, oldLeftGain, curLeftGain);
            oldLeftGain = curLeftGain;
        }
        else
        {
            buffer.applyGain (0, 0, numSamples, curLeftGain);
        }

        if (curRightGain != oldRightGain)
        {
            buffer.applyGainRamp (1, 0, numSamples, oldRightGain, curRightGain);
            oldRightGain = curRightGain;
        }
        else
        {
            buffer.applyGain (1, 0, numSamples, curRightGain);
        }
    }
}
