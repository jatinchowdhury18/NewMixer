#ifndef TRACKPROCESSOR_H_INCLUDED
#define TRACKPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"
#include "GainProcessor.h"
#include "PanProcessor.h"
#include "DelayProcessor.h"
#include "DistanceProcessor.h"
#include "ReverbProcessor.h"

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

    void trackMoved (int x, int y, int width, bool mouseUp);

    void setMute (bool mute) { isMute = mute; }
    bool getIsMute() { return isMute; }

    SoloState getSoloed() { return soloState; }
    void setSoloed (SoloState state) { soloState = state; }

    void rewind() { readerStartSample = 0; }

private:
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;

    std::unique_ptr<GainProcessor> gainProcessor;
    std::unique_ptr<DelayProcessor> delayProcessor;
    std::unique_ptr<PanProcessor> panProcessor;
    std::unique_ptr<DistanceProcessor> distProcessor;
    std::unique_ptr<ReverbProcessor> reverbProcessor;

    int64 readerStartSample = 0;
    bool isMute = true;
    SoloState soloState = noTracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED

