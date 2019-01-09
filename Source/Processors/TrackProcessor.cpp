#include "TrackProcessor.h"
#include "../Tracks/Track.h"
#include "../MainComponent.h"

namespace
{
    constexpr float roomWidth = 3.0f; //meters
    constexpr float headWidth = 0.215f; //meters

    constexpr float leftEarX = headWidth / 2.0f;
    constexpr float rightEarX = - leftEarX;

    constexpr float speedOfSound = 343.0f;
}

TrackProcessor::TrackProcessor (File& file) : ProcessorBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();

    reader.reset (formatManager.createReaderFor (file.createInputStream()));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());

    gainProcessor.reset (new GainProcessor);
    delayProcessor.reset (new DelayProcessor);
    panProcessor.reset (new PanProcessor);
    distProcessor.reset (new DistanceProcessor);
    reverbProcessor.reset (new ReverbProcessor);
}

void TrackProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);

    gainProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    delayProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    panProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    distProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
    reverbProcessor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);

    //Test Code
    //delayProcessor->setLengthMs (0, 1000.0);
    //reverbProcessor->setDryWet (1.0f);
}

void TrackProcessor::releaseResources()
{
    gainProcessor->releaseResources();
    delayProcessor->releaseResources();
    panProcessor->releaseResources();
    distProcessor->releaseResources();
    reverbProcessor->releaseResources();
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
    //reverbProcessor->processBlock (buffer, midiMessages);

    if (isMute || soloState == otherTrack)
        buffer.clear();
}

void TrackProcessor::trackMoved (int x, int y, int width, bool mouseUp)
{
    float gain = powf ((float) width / (float) Track::maxWidth, 5.0f);
    gainProcessor->setGain (gain);

    if (mouseUp) //adjust delay
    {
        const float trackX = (float) (x - MainComponent::width / 2) * roomWidth / (float) MainComponent::width;
        const float trackY = (float) (MainComponent::height - y) * roomWidth / (float) MainComponent::height;

        const float leftDelay  = sqrtf (powf (trackX - leftEarX, 2)  + powf (trackY, 2)) * 1000.0f / speedOfSound;
        const float rightDelay = sqrtf (powf (trackX - rightEarX, 2) + powf (trackY, 2)) * 1000.0f / speedOfSound;

        delayProcessor->setLengthMs (0, leftDelay);
        delayProcessor->setLengthMs (1, rightDelay);
    }

    float pan = (float) x / (float) MainComponent::width;
    panProcessor->setPan (pan);

    float distGain = (float) y / (float) MainComponent::height;
    distProcessor->setGain (distGain);

    float distFreq = (float) (FilterProcessor::farFreq) + distGain * (float) (FilterProcessor::maxFreq - FilterProcessor::farFreq);
    distProcessor->setFreq (distFreq);
}
