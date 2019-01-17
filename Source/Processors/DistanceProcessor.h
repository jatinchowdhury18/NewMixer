#ifndef DISTANCEPROCESSOR_H_INCLUDED
#define DISTANCEPROCESSOR_H_INCLUDED

#include "GainProcessor.h"
#include "FilterProcessor.h"
#include "ReverbProcessor.h"

class DistanceProcessor : public ProcessorBase
{
public:
    DistanceProcessor();

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setFreq (float newFreq) { filterProcessor->setFreq (newFreq); }
    void setGain (float newGain) { gainProcessor->setGain (newGain); }
    void setVerb (float wetAmt) { reverbProcessor->setDryWet (wetAmt); }

private:
    Array<AudioProcessor*> processors;
    std::unique_ptr<FilterProcessor> filterProcessor;
    std::unique_ptr<GainProcessor> gainProcessor;
    std::unique_ptr<ReverbProcessor> reverbProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistanceProcessor)
};

#endif //DISTANCEPROCESSOR_H_INCLUDED
