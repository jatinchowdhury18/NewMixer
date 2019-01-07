#ifndef DELAYPROCESSOR_H_INCLUDED
#define DELAYPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

class DelayProcessor : public ProcessorBase
{
public:
    DelayProcessor() : ProcessorBase (String ("Delay Processor")) {}

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setLengthMs (int channel, double lengthMs);

    double getTailLengthSeconds() const override;

private:
    enum
    {
        maxDelaySeconds = 10,
    };

    struct DelayChannel
    {
        LinearSmoothedValue<float> length = 0.0f;
        int writePtr = 0;

        void resetPtrs() { writePtr = 0; }
        void setWritePtr (int maxLength);
    };

    DelayChannel dChannels[2];

    AudioBuffer<float> delayBuffer;
    int bufferSize = 0;

    float delay (int channel, float x);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProcessor)
};

#endif //DELAYPROCESSOR_H_INCLUDED
