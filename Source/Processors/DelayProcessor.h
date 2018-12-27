#ifndef DELAYPROCESSOR_H_INCLUDED
#define DELAYPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

class DelayProcessor : public ProcessorBase
{
public:
    DelayProcessor() : ProcessorBase (String ("Delay Processor")) {}

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer);

    void setLengthMs (int channel, double lengthMs);

    double getTailLengthSeconds() const override;

private:
    void resetPtrs();
    void setReadPtr (int channel);
    void setWritePtr (int channel);

    int length[2] = {0, 0};
    int readPtr[2] = {0, 0}, writePtr[2] = {0, 0};

    AudioBuffer<float> delayBuffer;

    float delay (int channel, float x);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProcessor)
};

#endif DELAYPROCESSOR_H_INCLUDED
