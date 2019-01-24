#ifndef TRACKPROCESSOR_H_INCLUDED
#define TRACKPROCESSOR_H_INCLUDED

#include "TrackBase.h"

class TrackProcessor : public TrackBase
{
public:
    enum SoloState
    {
        thisTrack,
        otherTrack,
        noTracks,
    };

    TrackProcessor (File& file);
    TrackProcessor (MemoryInputStream* input);

    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
    
    int64 getLengthSamples() override { return reader->lengthInSamples; }

private:
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED
