#ifndef TRACKPROCESSOR_H_INCLUDED
#define TRACKPROCESSOR_H_INCLUDED

#include "JuceHeader.h"

class TrackProcessor : public AudioProcessor
{
public:
    TrackProcessor (File& file);

    const String getName() const override { return String ("Track Processor"); }

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

    double getTailLengthSeconds() const override { return 0.0; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }

    AudioProcessorEditor* createEditor() override { return nullptr; };
    bool hasEditor() const override { return false; }

    int getNumPrograms() override { return 0; }
    void setCurrentProgram (int /*index*/) override {}
    int getCurrentProgram() override { return 0; }

    const String getProgramName (int /*index*/) override { return String(); }
    void changeProgramName (int /*index*/, const String& /*newName*/) override {}

    void getStateInformation (MemoryBlock& /*destData*/) override {}
    void setStateInformation (const void* /*data*/, int /*sizeInBytes*/) override {}

private:
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReader> reader;

    int64 readerStartSample = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED

