#include "TrackProcessor.h"

TrackProcessor::TrackProcessor (File& file) : ProcessorBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();

    reader.reset (formatManager.createReaderFor (file.createInputStream()));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

void TrackProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    
}

void TrackProcessor::releaseResources()
{

}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    reader->read (&buffer, 0, buffer.getNumSamples(), readerStartSample, true, true);

    readerStartSample += buffer.getNumSamples();
    
    if (readerStartSample > reader->lengthInSamples)
        readerStartSample = 0;
}
