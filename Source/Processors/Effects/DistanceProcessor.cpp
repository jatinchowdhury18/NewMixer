#include "DistanceProcessor.h"

DistanceProcessor::DistanceProcessor() : ProcessorBase (String ("Filter Processor"))
{
    filterProcessor.reset (new FilterProcessor);
    processors.add (filterProcessor.get());

    gainProcessor.reset (new GainProcessor);
    processors.add (gainProcessor.get());
    
    splitterProcessor.reset (new SplitterProcessor);
    processors.add (splitterProcessor.get());
}

void DistanceProcessor::prepareToPlay (double sampleRate, int maxExpectedBlockSize)
{
    setRateAndBufferSizeDetails (sampleRate, maxExpectedBlockSize);

    for (auto* proc : processors)
        proc->prepareToPlay (sampleRate, maxExpectedBlockSize);
}

void DistanceProcessor::releaseResources()
{
    for (auto* proc : processors)
        proc->releaseResources();
}

void DistanceProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    for (auto* proc : processors)
        proc->processBlock (buffer, midiBuffer);
}
