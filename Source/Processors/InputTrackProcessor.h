#ifndef INPUTTRACKPROCESSOR_H_INCLUED
#define INPUTTRACKPROCESSOR_H_INCLUED

#include "TrackBase.h"

enum NumLoops
{
    Free = 0x9191,
    One,
    Two,
    Four
};

class InputTrackProcessor : public TrackBase
{
public:
    InputTrackProcessor (int64 len, int64 startSample);
    InputTrackProcessor (const InputTrackProcessor& processor);
    
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
    
    int64 getLengthSamples() const override { return inputBuffer.getNumSamples(); }
    
    bool isInputTrack() { return inputTrack; }
    bool isArmed() { return armed; }
    bool isRecording() { return recording; }
    void arm (NumLoops numLoops = Free, bool keyboardTrigger = false);
    void setRecordingStatus();
    void throwAway();
    
private:
    bool inputTrack = false;
    bool recording = false;
    bool armed = false;
    int loopsToRecord = 0;
    AudioBuffer<float> inputBuffer;
    
    JUCE_LEAK_DETECTOR (InputTrackProcessor)
};

#endif //INPUTTRACKPROCESSOR_H_INCLUED
