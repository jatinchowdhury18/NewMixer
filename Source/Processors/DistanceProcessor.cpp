#include "DistanceProcessor.h"

DistanceProcessor::DistanceProcessor() : ProcessorBase (String ("Filter Processor"))
{
    filterProcessor.reset (new FilterProcessor);
    gainProcessor.reset (new GainProcessor);
}

void DistanceProcessor::prepareToPlay (double sampleRate, int maxExpectedBlockSize)
{
    setRateAndBufferSizeDetails (sampleRate, maxExpectedBlockSize);

    filterProcessor->prepareToPlay (sampleRate, maxExpectedBlockSize);
    gainProcessor->prepareToPlay (sampleRate,maxExpectedBlockSize);
}

void DistanceProcessor::releaseResources()
{
    filterProcessor->releaseResources();
    gainProcessor->releaseResources();
}

void DistanceProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    filterProcessor->processBlock (buffer, midiBuffer);
    gainProcessor->processBlock (buffer, midiBuffer);
}
