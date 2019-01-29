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
    const auto numSamples = buffer.getNumSamples();
    if (readerStartSample + numSamples <= reader->lengthInSamples)
    {
        reader->read (&buffer, 0, numSamples, readerStartSample, true, true);
        readerStartSample += numSamples;
    }
    else
    {
        auto samplesUnder = reader->lengthInSamples - readerStartSample;
        reader->read (&buffer, 0, (int) samplesUnder, readerStartSample, true, true);
        reader->read (&buffer, (int) samplesUnder, numSamples - (int) samplesUnder, 0, true, true);
        readerStartSample = numSamples - samplesUnder;

        listeners.call (&TrackBase::Listener::newLoop);
    }

    TrackBase::processBlock (buffer, midiMessages);
}
