#ifndef PANPROCESSOR_H_INCLUDED
#define PANPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

/** Power law pan processor */
class PanProcessor : public ProcessorBase
{
public:
    PanProcessor();

    void setPan (float newPan);
    float getPan() const { return pan; }

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/) override;

private:
    void updateChannelGains();

    float pan = 0.5f; //0.5 = center, 0.0 = left, 1.0 = right

    float curLeftGain = 1.0f;
    float oldLeftGain = 1.0f;
    float curRightGain = 1.0f;
    float oldRightGain = 1.0f;

    const float halfPi = (float) double_Pi / 2.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanProcessor)
};

#endif //PANPROCESSOR_H_INCLUDED
