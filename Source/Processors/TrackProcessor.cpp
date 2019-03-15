#include "TrackProcessor.h"

TrackProcessor::TrackProcessor (File& file, int64 startSample) : TrackBase (String ("Track Processor"))
{
    readerStartSample = startSample;

    formatManager.registerBasicFormats();
    reader = formatManager.createReaderFor (file.createInputStream());

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

TrackProcessor::TrackProcessor (MemoryInputStream* input, int64 startSample) : TrackBase (String ("Track Processor"))
{
    readerStartSample = startSample;

    formatManager.registerBasicFormats();
    reader = formatManager.createReaderFor (input);

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

TrackProcessor::TrackProcessor (const TrackProcessor& processor) : TrackBase (String ("Track Processor"))
{
    readerStartSample = processor.getStartSample();

    formatManager.registerBasicFormats();
    auto* oldMis = dynamic_cast<MemoryInputStream*> (processor.getReader()->input);
    MemoryInputStream* mis = new MemoryInputStream (oldMis->getData(), oldMis->getDataSize(), false);
    reader = formatManager.createReaderFor (mis);

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (reader->lengthInSamples < 0)
        return; //@TODO: this should not be happening, ever.

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

        if (looping)
        {
            reader->read (&buffer, (int) samplesUnder, numSamples - (int) samplesUnder, 0, true, true);
            readerStartSample = numSamples - samplesUnder;

            listeners.call (&TrackBase::Listener::newLoop);
        }
        else
        {
            buffer.applyGainRamp (0, (int) samplesUnder, 1.0f, 0.0f);
            buffer.applyGain ((int) samplesUnder, numSamples - (int) samplesUnder, 0.0f);
            readerStartSample = reader->lengthInSamples;
        }
    }

    TrackBase::processBlock (buffer, midiMessages);
}
