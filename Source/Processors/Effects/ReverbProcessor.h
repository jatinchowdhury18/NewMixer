#ifndef REVERBPROCESSOR_H_INCLUDED
#define REVERBPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

class ReverbProcessor : public ProcessorBase
{
public:
    ReverbProcessor();

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setDryWet (float wetAmt);
    float getWet() const { return reverbDsp.getParameters().wetLevel; }

private:
    
    dsp::Reverb::Parameters params;
    dsp::Reverb reverbDsp;

    bool starting = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbProcessor)
};

#endif //REVERBPROCESSOR_H_INCLUDED
