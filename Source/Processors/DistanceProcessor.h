#ifndef DISTANCEPROCESSOR_H_INCLUDED
#define DISTANCEPROCESSOR_H_INCLUDED

#include "GainProcessor.h"
#include "FilterProcessor.h"

class DistanceProcessor : public ProcessorBase
{
public:
    DistanceProcessor();

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setFreq (float newFreq) { filterProcessor->setFreq (newFreq); }
    void setGain (float newGain) { gainProcessor->setGain (newGain); }

private:
    std::unique_ptr<FilterProcessor> filterProcessor;
    std::unique_ptr<GainProcessor> gainProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistanceProcessor)
};

#endif //DISTANCEPROCESSOR_H_INCLUDED
