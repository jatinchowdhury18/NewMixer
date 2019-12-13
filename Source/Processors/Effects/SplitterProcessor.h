#ifndef SPLITTERPROCESSOR_H_INCLUDED
#define SPLITTERPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

/** Splits main bus into dry and wet signals for reverb */
class SplitterProcessor : public ProcessorBase
{
public:
    SplitterProcessor();
    ~SplitterProcessor() {}

    void prepareToPlay (double sampleRate, int numSamples) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setWetGain (float newGain) { wetGain = newGain; }
    float getWetGain() { return wetGain; }

private:
    float wetGain;
    float oldWetGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplitterProcessor)
};

#endif //SPLITTERPROCESSOR_H_INCLUDED
