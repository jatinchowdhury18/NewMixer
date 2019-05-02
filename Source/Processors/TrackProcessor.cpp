#include "TrackProcessor.h"

TrackProcessor::TrackProcessor (File& file, int64 startSample) :
    TrackBase (String ("Track Processor")),
    file (file)
{
    readerStartSample = startSample;

    formatManager.registerBasicFormats();
    setupTransportSource (file.createInputStream());

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

TrackProcessor::TrackProcessor (MemoryInputStream* input, int64 startSample) : TrackBase (String ("Track Processor"))
{
    readerStartSample = startSample;

    formatManager.registerBasicFormats();
    setupTransportSource (input);

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

TrackProcessor::TrackProcessor (const TrackProcessor& processor) :
    TrackBase (processor)
{
    readerStartSample = processor.getStartSample();

    formatManager.registerBasicFormats();
    auto* oldMis = dynamic_cast<MemoryInputStream*> (processor.getReader()->input);
    if (oldMis != nullptr)
    {
        MemoryInputStream* mis = new MemoryInputStream (oldMis->getData(), oldMis->getDataSize(), false);
        setupTransportSource (mis);
    }

    file = processor.file;
    if (file.exists())
        setupTransportSource (file.createInputStream());

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());
}

void TrackProcessor::setFile (File& newFile)
{
    file = newFile;

    delete reader;
    setupTransportSource (file.createInputStream());
}

void TrackProcessor::setupTransportSource (InputStream* inputStream)
{
    reader = formatManager.createReaderFor (inputStream);
    readerSource.reset (new AudioFormatReaderSource (reader, false));
    transportSource.setSource (readerSource.get(), 0, nullptr, reader->sampleRate);
}

void TrackProcessor::prepareToPlay (double sampleRate, int maxExpectedBlockSize)
{
    TrackBase::prepareToPlay (sampleRate, maxExpectedBlockSize);

    transportSource.prepareToPlay (maxExpectedBlockSize, sampleRate);
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (! transportSource.isPlaying())
        transportSource.start();

    const auto numSamples = buffer.getNumSamples();
    AudioSourceChannelInfo channelInfo (buffer);
    transportSource.setNextReadPosition (readerStartSample);
    if (readerStartSample + numSamples <= transportSource.getTotalLength())
    {
        transportSource.getNextAudioBlock (channelInfo);
        readerStartSample += numSamples;
    }
    else
    {
        auto samplesUnder = jmax<int64> (transportSource.getTotalLength() - readerStartSample, 0);

        transportSource.getNextAudioBlock (channelInfo);

        if (looping)
        {
            channelInfo.startSample = (int) samplesUnder;
            channelInfo.numSamples = numSamples - (int) samplesUnder;
            transportSource.setNextReadPosition (0);
            transportSource.getNextAudioBlock (channelInfo);
            readerStartSample = numSamples - samplesUnder;

            listeners.call (&TrackBase::Listener::newLoop);
        }
        else
        {
            buffer.applyGainRamp (0, (int) samplesUnder, 1.0f, 0.0f);
            buffer.applyGain ((int) samplesUnder, numSamples - (int) samplesUnder, 0.0f);
            readerStartSample = transportSource.getTotalLength();

            transportSource.stop();

            listeners.call (&TrackBase::Listener::endReached);
        }
    }

    if (reader->numChannels < 2)
        buffer.copyFrom (1, 0, buffer.getReadPointer(0), buffer.getNumSamples());

    TrackBase::processBlock (buffer, midiMessages);
}

void TrackProcessor::releaseResources()
{
    TrackBase::releaseResources();
    transportSource.stop();
    transportSource.releaseResources();
}
