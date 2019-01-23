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
    TrackProcessor (MemoryInputStream* input);
    TrackProcessor (int64 len);

    void initProcessors();

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

    void trackMoved (int x, int y, int width, bool mouseUp);

    void setMute (bool mute) { isMute = mute; }
    bool getIsMute() { return isMute; }

    SoloState getSoloed() { return soloState; }
    void setSoloed (SoloState state) { soloState = state; }

    void rewind() { readerStartSample = 0; }
    float getRMSLevel() { return lastRMS; }

    int64 getLengthSamples() { return reader->lengthInSamples; }
    bool isInputTrack() { return inputTrack; }
    bool isArmed() { return armed; }
    bool isRecording() { return recording; }
    void arm() { armed = true; }
    void setRecordingStatus();

    class Listener
    {
    public:
        virtual void newLoop() {}
    };

    void addListener (Listener* listener) { listeners.add (listener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }

private:
    ListenerList<Listener> listeners;

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;

    Array<AudioProcessor*> processors;
    std::unique_ptr<GainProcessor> gainProcessor;
    std::unique_ptr<DelayProcessor> delayProcessor;
    std::unique_ptr<PanProcessor> panProcessor;
    std::unique_ptr<DistanceProcessor> distProcessor;

    int64 readerStartSample = 0;
    bool isMute = true;
    SoloState soloState = noTracks;
    float lastRMS = 0.0f;

    bool inputTrack = false;
    bool recording = false;
    bool armed = false;
    AudioBuffer<float> inputBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif // !TRACKPROCESSOR_H_INCLUDED

