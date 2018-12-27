#include "DelayProcessor.h"

void DelayProcessor::setReadPtr (int channel) {
    readPtr[channel]++;
    if (readPtr[channel] >= length[channel]) { readPtr[channel] = 0; } //wrap
}

void DelayProcessor::setWritePtr (int channel) {
    writePtr[channel]++;
    if (writePtr[channel] >= length[channel]) { writePtr[channel] = 0; } //wrap
}

double DelayProcessor::getTailLengthSeconds() const
{
    int maxLengthSamples = jmax<int> (length[0], length[1]);
    return (double) maxLengthSamples / getSampleRate();
}

void DelayProcessor::setLengthMs (int channel, double lengthMs)
{
    int newLength = (int) (lengthMs * getSampleRate() / 1000.0);
    length[channel] = newLength;
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
    
    resetPtrs();
}

void DelayProcessor::resetPtrs()
{ 
    readPtr[0] = 0; writePtr[0] = 0; 
    readPtr[1] = 0; writePtr[1] = 0; 
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
    float y = delayBuffer.getSample (channel, readPtr[channel]);	//read from buffer
    delayBuffer.setSample (channel, writePtr[channel], x);			//write to buffer
    
    //update pointers
    setReadPtr (channel);		
    setWritePtr (channel);
    return y;
}
