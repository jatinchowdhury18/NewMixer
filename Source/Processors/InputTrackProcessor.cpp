#include "InputTrackProcessor.h"

InputTrackProcessor::InputTrackProcessor (int64 len, int64 startSample) :
    TrackBase (String ("Input Track Processor"))
{
    readerStartSample = startSample;
    
    inputTrack = true;
    inputBuffer.setSize (2, (int) len);
    inputBuffer.clear();

    setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());
}

void InputTrackProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    if (readerStartSample + buffer.getNumSamples() > inputBuffer.getNumSamples())
    {
        listeners.call (&TrackBase::Listener::newLoop);
        readerStartSample = 0;
    }
    
    if (recording)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ch++)
            inputBuffer.copyFrom (ch, (int) readerStartSample, buffer, ch, 0, buffer.getNumSamples());
    }
    else
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ch++)
            buffer.copyFrom (ch, 0, inputBuffer, ch, (int) readerStartSample, buffer.getNumSamples());
    }
    
    readerStartSample += buffer.getNumSamples();
    
    TrackBase::processBlock(buffer, midiMessages);
}

void InputTrackProcessor::arm (NumLoops numLoops, bool keyboardTrigger)
{ 
    if (keyboardTrigger)
        armed = ! armed;
    else
        armed = true;

    loopsToRecord = numLoops - Free; 
}

void InputTrackProcessor::setRecordingStatus()
{
    if (armed)
    {
        inputBuffer.clear();
        
        armed = false;
        recording = true;
        return;
    }
    if (recording)
    {
        loopsToRecord--;
        if (loopsToRecord == 0)
            recording = false;

        return;
    }
}

void InputTrackProcessor::throwAway()
{
    inputBuffer.clear();

    if (armed)
        armed = false;

    if (recording)
    {
        recording = false;
        loopsToRecord = 0;
    }
}
