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

InputTrackProcessor::InputTrackProcessor (const InputTrackProcessor& processor) : 
    TrackBase (String ("Input Track Processor"))
{
    readerStartSample = processor.getStartSample();

    inputTrack = true;
    inputBuffer.makeCopyOf (processor.inputBuffer, false);

    setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());
}

void InputTrackProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    const auto numSamples = buffer.getNumSamples();
    if (readerStartSample + numSamples <= inputBuffer.getNumSamples())
    {
        if (recording)
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                inputBuffer.copyFrom (ch, (int) readerStartSample, buffer, ch, 0, numSamples);
        }
        else
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                buffer.copyFrom (ch, 0, inputBuffer, ch, (int) readerStartSample, numSamples);
        }
        readerStartSample += numSamples;
    }
    else
    {
        auto samplesUnder = inputBuffer.getNumSamples() - readerStartSample;
        if (recording)
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
            {
                inputBuffer.copyFrom (ch, (int) readerStartSample, buffer, ch, 0, (int) samplesUnder);
                inputBuffer.copyFrom (ch, 0, buffer, ch, (int) samplesUnder, numSamples - (int) samplesUnder);
            }
        }
        else
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
            {
                buffer.copyFrom (ch, 0, inputBuffer, ch, (int) readerStartSample, (int) samplesUnder);
                buffer.copyFrom (ch, (int) samplesUnder, inputBuffer, ch, 0, numSamples - (int) samplesUnder);
            }
        }
        readerStartSample = numSamples - samplesUnder;

        listeners.call (&TrackBase::Listener::newLoop);
    }
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
