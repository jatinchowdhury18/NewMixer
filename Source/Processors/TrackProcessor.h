#ifndef TRACKPROCESSOR_H_INCLUDED
#define TRACKPROCESSOR_H_INCLUDED

#include "JuceHeader.h"
#include "ProcessorBase.h"

class TrackProcessor : public ProcessorBase
{
public:
    TrackProcessor (File& file);

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

private:
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReader> reader;

    int64 readerStartSample = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED

