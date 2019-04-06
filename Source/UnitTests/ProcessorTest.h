#ifndef PROCESSORTEST_H_INCLUDED
#define PROCESSORTEST_H_INCLUDED

#include "InputTrackProcessor.h"

class ProcessorTest : public UnitTest
{
    enum TestConstants
    {
        width = 900,
        height = 600,
        heightScaled = (int) (height * MainConstants::heightFactor),
    };

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
        processor->trackMoved (0, 0, 50, width, height);
        checkProcessors (0.03125f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (0, 0, 100, width, height);
        checkProcessors (1.0f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (0, 0, 0, width, height);
        checkProcessors (0.0f, 9.6426f, 9.9228f, 0.0f, 0.0f, 1000.0f, 1.0f);

        beginTest ("Delay Processor");
        processor->trackMoved (width, 0, 50, width, height);
        checkProcessors (0.03125f, 9.9228f, 9.6426f, 1.0f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (width, heightScaled, 50, width, height);
        checkProcessors (0.03125f, 4.6866f, 4.0598f, 1.0f, 1.0f, 22000.0f, 0.0f);

        processor->trackMoved (0, heightScaled, 50, width, height);
        checkProcessors (0.03125f, 4.0598f, 4.6866f, 0.0f, 1.0f, 22000.0f, 0.0f);

        beginTest ("Pan Processor");
        processor->trackMoved (width / 2, 0, 50, width, height);
        checkProcessors (0.03125f, 8.752f, 8.752f, 0.5f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (width / 4, 0, 50, width, height);
        checkProcessors (0.03125f, 8.9447f, 9.0966f,  0.25f, 0.0f, 1000.0f, 1.0f);

        processor->trackMoved (3 * width / 4, 0, 50, width, height);
        checkProcessors (0.03125f, 9.0966f, 8.9447f, 0.75f, 0.0f, 1000.0f, 1.0f);

        beginTest ("Dist Processor");
        processor->trackMoved (width / 2, heightScaled / 2, 50, width, height);
        checkProcessors (0.03125f, 4.3844f, 4.3844f, 0.5f, 0.5f, 6250.0f, 0.0625f);

        processor->trackMoved (width / 2, heightScaled / 4, 50, width, height);
        checkProcessors (0.03125f, 6.5673f, 6.5673f, 0.5f, 0.25f, 2312.5f, 0.31640625f);

        processor->trackMoved (width / 2, 3 * heightScaled / 4, 50, width, height);
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

#endif //PROCESSORTEST_H_INCLUDED
