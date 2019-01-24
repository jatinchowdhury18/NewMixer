#ifndef INPUTTRACKPROCESSOR_H_INCLUED
#define INPUTTRACKPROCESSOR_H_INCLUED

#include "TrackBase.h"

class InputTrackProcessor : public TrackBase
{
public:
    InputTrackProcessor (int64 len, int64 startSample);
    
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
    
    int64 getLengthSamples() override { return inputBuffer.getNumSamples(); }
    
    bool isInputTrack() { return inputTrack; }
    bool isArmed() { return armed; }
    bool isRecording() { return recording; }
    void arm() { armed = true; }
    void setRecordingStatus();
    
private:
    bool inputTrack = false;
    bool recording = false;
    bool armed = false;
    AudioBuffer<float> inputBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputTrackProcessor)
};

#endif //INPUTTRACKPROCESSOR_H_INCLUED
