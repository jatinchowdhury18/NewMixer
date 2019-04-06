#include "TrackBase.h"
#include "Track.h"
#include "MainComponent.h"

namespace
{
    constexpr float roomWidth = 3.0f; //meters
    constexpr float headWidth = 0.215f; //meters
    
    constexpr float leftEarX = - headWidth / 2.0f;
    constexpr float rightEarX = - leftEarX;
    
    constexpr float speedOfSound = 343.0f;
}

TrackBase::TrackBase (String name) : ProcessorBase (name)
{
    initProcessors();
}

void TrackBase::initProcessors()
{
    gainProcessor.reset (new GainProcessor);
    processors.add (gainProcessor.get());
    
    delayProcessor.reset (new DelayProcessor);
    processors.add (delayProcessor.get());
    
    panProcessor.reset (new PanProcessor);
    processors.add (panProcessor.get());
    
    distProcessor.reset (new DistanceProcessor);
    processors.add (distProcessor.get());
}

void TrackBase::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, maximumExpectedSamplesPerBlock);
    
    for (auto* processor : processors)
        processor->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
}

void TrackBase::releaseResources()
{
    for (auto* processor : processors)
        processor->releaseResources();
}

void TrackBase::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    for (auto* processor : processors)
        processor->processBlock (buffer, midiMessages);
    
    if (isMute || soloState == otherTrack)
        buffer.clear();
    
    float rmsSum = 0;
    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
        rmsSum += buffer.getRMSLevel (ch, 0, buffer.getNumSamples());
    
    lastRMS = rmsSum / buffer.getNumChannels();
}

void TrackBase::trackMoved (int x, int y, int width, int screenWidth, int screenHeight)
{
    updateGain (width);
    updateDelay (x, y, screenWidth, screenHeight);
    updatePan (x, screenWidth);
    updateDist (y, screenHeight);
}

void TrackBase::updateGain (int width)
{
    float gain = powf ((float) width / TrackConstants::maxDiameter, 5.0f);
    gainProcessor->setGain (gain);
}

void TrackBase::updateDelay (int x, int y, int screenWidth, int screenHeight)
{
    const auto correctHeight = (float) screenHeight * MainConstants::heightFactor;
    const auto trackX = (float) (x - screenWidth / 2) * roomWidth / (float) screenWidth;
    const auto trackY = (float) (correctHeight - y) * roomWidth / correctHeight;

    const auto leftDelay  = sqrtf (powf (trackX - leftEarX, 2)  + powf (trackY, 2)) * 1000.0f / speedOfSound;
    const auto rightDelay = sqrtf (powf (trackX - rightEarX, 2) + powf (trackY, 2)) * 1000.0f / speedOfSound;

    delayProcessor->setLengthMs (0, leftDelay);
    delayProcessor->setLengthMs (1, rightDelay);
}

void TrackBase::updatePan (int x, int screenWidth)
{
    const auto pan = (float) x / (float) screenWidth;
    panProcessor->setPan (pan);
}

void TrackBase::updateDist (int y, int screenHeight)
{
    const auto distFactor = (float) y / ((float) screenHeight * MainConstants::heightFactor);
    distProcessor->setGain (distFactor);

    const auto distFreq = (float) (FilterProcessor::farFreq) + powf (distFactor, 2.0f) * (float) (FilterProcessor::maxFreq - FilterProcessor::farFreq);
    distProcessor->setFreq (distFreq);

    const auto verbFactor = powf (1.0f - distFactor, 4.0f);
    distProcessor->setVerb (verbFactor);
}

#if JUCE_DEBUG
#include "UnitTests/ProcessorTest.h"
static ProcessorTest procTest;
#endif
