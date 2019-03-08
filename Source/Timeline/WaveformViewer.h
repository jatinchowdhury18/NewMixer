#ifndef WAVEFORMVIEWER_H_INCLUDED
#define WAVEFORMVIEWER_H_INCLUDED

#include "Playhead.h"
//#include "MarkerView.h"
//#include "JuceHeader.h"
#include "Processors/TrackProcessor.h"

class WaveformViewer : public Component
{
public:
    WaveformViewer (TrackProcessor* trackProc);
    ~WaveformViewer();

    void paint (Graphics&) override;
    void resized() override;

private:
    AudioThumbnailCache cache;
    AudioThumbnail waveform;

    Playhead playhead;
    //MarkerView marker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformViewer)
};

#endif //WAVEFORMVIEWER_H_INCLUDED
