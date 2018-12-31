#include "DelayProcessor.h"

void DelayProcessor::DelayChannel::setReadPtr()
{
    readPtr++;
    if (readPtr >= length) //wrap
        readPtr = 0;
}

void DelayProcessor::DelayChannel::setWritePtr()
{
    writePtr++;
    if (writePtr >= length) //wrap
        writePtr = 0;
}

double DelayProcessor::getTailLengthSeconds() const
{
    int maxLengthSamples = 0;
    for (auto ch : dChannels)
        maxLengthSamples = jmax<int> (ch.length, maxLengthSamples);

    return (double) maxLengthSamples / getSampleRate();
}

void DelayProcessor::setLengthMs (int channel, double lengthMs)
{
    int newLength = (int) (lengthMs * getSampleRate() / 1000.0);
    dChannels[channel].length = newLength;
}

void DelayProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);
    delayBuffer.setSize (2, (int) sampleRate * 10);

    delayBuffer.clear();
}

void DelayProcessor::releaseResources()
{
    delayBuffer.clear();
    
    for (auto ch : dChannels)
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
    float y = delayBuffer.getSample (channel, dChannels[channel].readPtr);	//read from buffer
    delayBuffer.setSample (channel, dChannels[channel].writePtr, x);			//write to buffer
    
    //update pointers
    dChannels[channel].setReadPtr();
    dChannels[channel].setWritePtr();

    return y;
}
