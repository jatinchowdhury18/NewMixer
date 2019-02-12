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

void TrackBase::trackMoved (int x, int y, int width)
{
    updateGain (width);
    updateDelay (x, y);
    updatePan (x);
    updateDist (y);
}

void TrackBase::updateGain (int width)
{
    float gain = powf ((float) width / TrackConstants::maxDiameter, 5.0f);
    gainProcessor->setGain (gain);
}

void TrackBase::updateDelay (int x, int y)
{
    const auto trackX = (float) (x - MainComponent::width / 2) * roomWidth / (float) MainComponent::width;
    const auto trackY = (float) (MainComponent::height - y) * roomWidth / (float) MainComponent::height;

    const auto leftDelay  = sqrtf (powf (trackX - leftEarX, 2)  + powf (trackY, 2)) * 1000.0f / speedOfSound;
    const auto rightDelay = sqrtf (powf (trackX - rightEarX, 2) + powf (trackY, 2)) * 1000.0f / speedOfSound;

    delayProcessor->setLengthMs (0, leftDelay);
    delayProcessor->setLengthMs (1, rightDelay);
}

void TrackBase::updatePan (int x)
{
    const auto pan = (float) x / (float) MainComponent::width;
    panProcessor->setPan (pan);
}

void TrackBase::updateDist (int y)
{
    const auto distFactor = (float) y / (float) MainComponent::height;
    distProcessor->setGain (distFactor);

    const auto distFreq = (float) (FilterProcessor::farFreq) + powf (distFactor, 2.0f) * (float) (FilterProcessor::maxFreq - FilterProcessor::farFreq);
    distProcessor->setFreq (distFreq);

    const auto verbFactor = powf (1.0f - distFactor, 4.0f);
    distProcessor->setVerb (verbFactor);
}

#if JUCE_DEBUG
#include "InputTrackProcessor.h"
class ProcessorTest : public UnitTest
{
public:
    ProcessorTest() : UnitTest ("Test Processors") {}

    void checkProcessors (float gain, float delayL, float delayR, float pan, float distGain, float distFreq, float distVerb)
    {
        expectEquals<float> (processor->gainProcessor->getGain(), gain, "Processor Gain incorrect");

        expectWithinAbsoluteError<float> (processor->delayProcessor->getLengthMS (0), delayL, precise, "Processor Left Delay incorrect");
        expectWithinAbsoluteError<float> (processor->delayProcessor->getLengthMS (1), delayR, precise, "Processor Right Delay incorrect");

        expectEquals<float> (processor->panProcessor->getPan(), pan, "Processor Pan Incorrect");

        expectEquals<float> (processor->distProcessor->getGain(), distGain, "Processor Dist Gain Incorrect");
        expectEquals<float> (processor->distProcessor->getFreq(), distFreq, "Processor Dist Freq Incorrect");
        expectEquals<float> (processor->distProcessor->getVerb(), distVerb, "Processor Dist Verb Incorrect");
    }

    void runTest() override
    {
        beginTest ("Gain Processor");
        processor->trackMoved (0, 0, 50);
        checkProcessors (0.03125f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (0, 0, 100);
        checkProcessors (1.0f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (0, 0, 0);
        checkProcessors (0.0f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        beginTest ("Delay Processor");
        processor->trackMoved (MainComponent::width, 0, 50);
        checkProcessors (0.03125f, 9.9228f, 9.6426f, 1.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (MainComponent::width, MainComponent::height, 50);
        checkProcessors (0.03125f, 4.6866f, 4.0598f, 1.0f, 1.0f, 22000.0f, 0.0f);

        processor->trackMoved (0, MainComponent::height, 50);
        checkProcessors (0.03125f, 4.0598f, 4.6866f, 0.0f, 1.0f, 22000.0f, 0.0f);

        beginTest ("Pan Processor");
        processor->trackMoved (MainComponent::width / 2, 0, 50);
        checkProcessors (0.03125f, 8.752f, 8.752f, 0.5f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (MainComponent::width / 4, 0, 50);
        checkProcessors (0.03125f, 8.9447f, 9.0966f,  0.25f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (3 * MainComponent::width / 4, 0, 50);
        checkProcessors (0.03125f, 9.0966f, 8.9447f, 0.75f, 0.0f, 1000.0f, 1.0f);

        beginTest ("Dist Processor");
        processor->trackMoved (MainComponent::width / 2, MainComponent::height / 2, 50);
        checkProcessors (0.03125f, 4.3844f, 4.3844f, 0.5f, 0.5f, 6250.0f, 0.0625f);

        processor->trackMoved (MainComponent::width / 2, MainComponent::height / 4, 50);
        checkProcessors (0.03125f, 6.5673f, 6.5673f, 0.5f, 0.25f, 2312.5f, 0.31640625f);

        processor->trackMoved (MainComponent::width / 2, 3 * MainComponent::height / 4, 50);
        checkProcessors (0.03125f, 2.2089f, 2.2089f, 0.5f, 0.75f, 12812.5f, 0.00390625f);
    }

    void initialise() override
    { 
        processor = new InputTrackProcessor (100, 0);
        processor->prepareToPlay (44100.0, 480);
    }
    void shutdown() override 
    {
        delete processor;
    }

private:
    TrackBase* processor;
    float precise = 0.05f;
};

static ProcessorTest procTest;
#endif
