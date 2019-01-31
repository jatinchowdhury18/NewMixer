#include "DelayProcessor.h"

void DelayProcessor::DelayChannel::setReadPtr (int maxLength)
{
    readPtr++;
    if (readPtr >= maxLength) //wrap
        readPtr = 0;
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
        ch.length.reset (sampleRate, 1e-1);
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
    
    // Erase head
    const int erasePtr = negativeAwareModulo(ch.readPtr - 1, bufferSize);
    delayBuffer.setSample (channel, erasePtr, 0.0f);

    // Write head
    const float len = ch.length.getNextValue();
    const float fractionSample = len - (int) len;
    const int writePtr = (ch.readPtr + (int) floorf (len)) % bufferSize;

    delayBuffer.addSample (channel, writePtr, x * (1.0f - fractionSample));
    delayBuffer.addSample (channel, (writePtr + 1) % bufferSize, x * fractionSample);

    // Read head
    float y = delayBuffer.getSample (channel, ch.readPtr);

    //update pointers
    ch.setReadPtr (bufferSize);

    return y;
}
