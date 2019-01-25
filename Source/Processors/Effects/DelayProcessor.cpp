#include "DelayProcessor.h"

void DelayProcessor::DelayChannel::setWritePtr (int maxLength)
{
    writePtr++;
    if (writePtr >= maxLength) //wrap
        writePtr = 0;
}

double DelayProcessor::getTailLengthSeconds() const
{
    int maxLengthSamples = 0;
    for (auto& ch : dChannels)
        maxLengthSamples = jmax ((int) ch.length.getTargetValue(), maxLengthSamples);

    return (double) maxLengthSamples / getSampleRate();
}

void DelayProcessor::setLengthMs (int channel, double lengthMs)
{
    int newLength = (int) (lengthMs * getSampleRate() / 1000.0);
    newLength = jmin (newLength, bufferSize);
    dChannels[channel].length.setValue ((float) newLength);
}

void DelayProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);
    bufferSize = (int) sampleRate * maxDelaySeconds;
    delayBuffer.setSize (2, bufferSize);
    delayBuffer.clear();

    for (auto& ch : dChannels)
        ch.length.reset (sampleRate, 1e-2);
}

void DelayProcessor::releaseResources()
{
    delayBuffer.clear();
    
    for (auto& ch : dChannels)
        ch.resetPtrs();
}

void DelayProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiBuffer*/)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        float* x = buffer.getWritePointer(channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
            x[n] = delay (channel, x[n]);  
    }
}

float DelayProcessor::delay (int channel, float x)
{
    auto& ch = dChannels[channel];
    float y = x;

    const float len = ch.length.getNextValue();
    const float fractionSample = len - (int) len;
    const int readPtr = (int) (ch.writePtr - len + bufferSize) % bufferSize;

    if (readPtr != ch.writePtr)
    {
        float samp0 = delayBuffer.getSample (channel, readPtr);
        float samp1 = delayBuffer.getSample (channel, (readPtr + 1) % bufferSize);
        y = samp0 + fractionSample * (samp1 - samp0);

        delayBuffer.setSample (channel, ch.writePtr, x);	    //write to buffer
    }

    //update pointers
    ch.setWritePtr (bufferSize);

    return y;
}
