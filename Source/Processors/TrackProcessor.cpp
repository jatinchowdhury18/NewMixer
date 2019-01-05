#include "TrackProcessor.h"
#include "../Tracks/Track.h"
#include "../MainComponent.h"

TrackProcessor::TrackProcessor (File& file) : ProcessorBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();

    reader.reset (formatManager.createReaderFor (file.createInputStream()));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());

    gainProcessor.reset (new GainProcessor);
    delayProcessor.reset (new DelayProcessor);
    panProcessor.reset (new PanProcessor);
    distProcessor.reset (new GainProcessor);
    reverbProcessor.reset (new ReverbProcessor);
    filterProcessor.reset (new FilterProcessor);
}

void TrackProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    gainProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    delayProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    panProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    distProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    reverbProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    filterProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);

    //Test Code
    //delayProcessor->setLengthMs (0, 1000.0);
    //reverbProcessor->setDryWet (1.0f);
    //filterProcessor->setFreq (1000.0f);
}

void TrackProcessor::releaseResources()
{
    gainProcessor->releaseResources();
    delayProcessor->releaseResources();
    panProcessor->releaseResources();
    distProcessor->releaseResources();
    reverbProcessor->releaseResources();
    filterProcessor->releaseResources();
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    reader->read (&buffer, 0, buffer.getNumSamples(), readerStartSample, true, true);

    readerStartSample += buffer.getNumSamples();
    
    if (readerStartSample > reader->lengthInSamples)
        readerStartSample = 0;

    gainProcessor->processBlock (buffer, midiMessages);
    delayProcessor->processBlock (buffer, midiMessages);
    panProcessor->processBlock (buffer, midiMessages);
    distProcessor->processBlock (buffer, midiMessages);
    reverbProcessor->processBlock (buffer, midiMessages);
    filterProcessor->processBlock (buffer, midiMessages);

    if (isMute || soloState == otherTrack)
        buffer.clear();
}

void TrackProcessor::trackMoved (int x, int y, int width)
{
    float gain = powf ((float) width / (float) Track::maxWidth, 5.0f);
    gainProcessor->setGain (gain);

    float pan = (float) x / (float) MainComponent::width;
    panProcessor->setPan (pan);

    float distGain = (float) y / (float) MainComponent::height;
    distProcessor->setGain (distGain);
}
