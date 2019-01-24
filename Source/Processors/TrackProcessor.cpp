#include "TrackProcessor.h"

TrackProcessor::TrackProcessor (File& file) : TrackBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();
    reader.reset (formatManager.createReaderFor (file.createInputStream()));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

TrackProcessor::TrackProcessor (MemoryInputStream* input) : TrackBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();
    reader.reset (formatManager.createReaderFor (input));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{

    if (readerStartSample > reader->lengthInSamples)
    {
        listeners.call (&TrackBase::Listener::newLoop);
        readerStartSample = 0;
    }

    reader->read (&buffer, 0, buffer.getNumSamples(), readerStartSample, true, true);
    
    readerStartSample += buffer.getNumSamples();

    TrackBase::processBlock (buffer, midiMessages);
}
