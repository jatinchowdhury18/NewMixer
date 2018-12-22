#include "TrackProcessor.h"
#include "../Tracks/Track.h"
#include "../MainComponent.h"

TrackProcessor::TrackProcessor (File& file) : ProcessorBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();

    reader.reset (formatManager.createReaderFor (file.createInputStream()));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());

    gainProcessor = new GainProcessor;
    panProcessor = new PanProcessor;
}

void TrackProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    gainProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    panProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
}

void TrackProcessor::releaseResources()
{
    gainProcessor->releaseResources();
    panProcessor->releaseResources();
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    reader->read (&buffer, 0, buffer.getNumSamples(), readerStartSample, true, true);

    readerStartSample += buffer.getNumSamples();
    
    if (readerStartSample > reader->lengthInSamples)
        readerStartSample = 0;

    gainProcessor->processBlock (buffer, midiMessages);
    panProcessor->processBlock (buffer, midiMessages);
}

void TrackProcessor::trackMoved (int x, int y, int width)
{
    float gain = powf ((float) width / (float) Track::maxWidth, 3.0f);
    gainProcessor->setGain (gain);

    float pan = (float) x / (float) MainComponent::width;
    panProcessor->setPan (pan);
}
