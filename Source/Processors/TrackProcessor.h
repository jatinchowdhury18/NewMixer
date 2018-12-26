#ifndef TRACKPROCESSOR_H_INCLUDED
#define TRACKPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"
#include "GainProcessor.h"
#include "PanProcessor.h"

class TrackProcessor : public ProcessorBase
{
public:
    enum SoloState
    {
        thisTrack,
        otherTrack,
        noTracks,
    };

    TrackProcessor (File& file);

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

    void trackMoved (int x, int y, int width);

    void setMute (bool mute) { isMute = mute; }
    bool getIsMute() { return isMute; }

    SoloState getSoloed() { return soloState; }
    void setSoloed (SoloState state) { soloState = state; }

    void rewind() { readerStartSample = 0; }

private:
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReader> reader;

    ScopedPointer<GainProcessor> gainProcessor;
    ScopedPointer<PanProcessor> panProcessor;
    ScopedPointer<GainProcessor> distProcessor;

    int64 readerStartSample = 0;
    bool isMute = true;
    SoloState soloState = noTracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED

