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
    
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
    
    int64 getLengthSamples() override { return inputBuffer.getNumSamples(); }
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputTrackProcessor)
};

#endif //INPUTTRACKPROCESSOR_H_INCLUED
