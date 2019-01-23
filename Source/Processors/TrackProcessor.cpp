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

    initProcessors();
}

TrackProcessor::TrackProcessor (MemoryInputStream* input) : ProcessorBase (String ("Track Processor"))
{
    formatManager.registerBasicFormats();

    reader.reset (formatManager.createReaderFor (input));

    setPlayConfigDetails (0, 2, getSampleRate(), getBlockSize());

    initProcessors();
}

TrackProcessor::TrackProcessor (int64 len, int64 startSample) : 
    ProcessorBase (String ("Track Processor")),
    readerStartSample (startSample)
{
    formatManager.registerBasicFormats();

    setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    inputTrack = true;
    inputBuffer.setSize (2, (int) len);
    inputBuffer.clear();

    initProcessors();
}

void TrackProcessor::initProcessors()
{
    gainProcessor.reset (new GainProcessor);
    processors.add (gainProcessor.get());

    delayProcessor.reset (new DelayProcessor);
    //processors.add (delayProcessor.get());

    panProcessor.reset (new PanProcessor);
    processors.add (panProcessor.get());

    distProcessor.reset (new DistanceProcessor);
    processors.add (distProcessor.get());
}

void TrackProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);

    for (auto* processor : processors)
        processor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);

    //Test Code
    //delayProcessor->setLengthMs (0, 1000.0);
    //reverbProcessor->setDryWet (1.0f);
}

void TrackProcessor::releaseResources()
{
    for (auto* processor : processors)
        processor->releaseResources();
}

void TrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (! inputTrack)
    {
        if (readerStartSample > reader->lengthInSamples)
        {
            listeners.call (&TrackProcessor::Listener::newLoop);
            readerStartSample = 0;
        }

        reader->read (&buffer, 0, buffer.getNumSamples(), readerStartSample, true, true);
    }
    else
    {
        if (readerStartSample + buffer.getNumSamples() > inputBuffer.getNumSamples())
        {
            listeners.call (&TrackProcessor::Listener::newLoop);
            readerStartSample = 0;
        }

        if (recording)
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                inputBuffer.copyFrom (ch, (int) readerStartSample, buffer, ch, 0, buffer.getNumSamples());
        }
        else
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                buffer.copyFrom (ch, 0, inputBuffer, ch, (int) readerStartSample, buffer.getNumSamples());
        }

    }

    readerStartSample += buffer.getNumSamples();

    for (auto* processor : processors)
        processor->processBlock (buffer, midiMessages);

    if (isMute || soloState == otherTrack)
        buffer.clear();

    float rmsSum = 0;
    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
        rmsSum += buffer.getRMSLevel (ch, 0, buffer.getNumSamples());
    
    lastRMS = rmsSum / buffer.getNumChannels();
}

void TrackProcessor::trackMoved (int x, int y, int width, bool mouseUp)
{
    float gain = powf ((float) width / TrackConstants::maxDiameter, 5.0f);
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

    float distFactor = (float) y / (float) MainComponent::height;
    distProcessor->setGain (distFactor);

    float distFreq = (float) (FilterProcessor::farFreq) + distFactor * (float) (FilterProcessor::maxFreq - FilterProcessor::farFreq);
    distProcessor->setFreq (distFreq);

    distProcessor->setVerb (1.0f - distFactor);
}

void TrackProcessor::setRecordingStatus()
{
    if (armed)
    {
        inputBuffer.clear();

        armed = false;
        recording = true;
        return;
    }
    if (recording)
    {
        recording = false;
        return;
    }
}
