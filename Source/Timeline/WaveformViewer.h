#ifndef WAVEFORMVIEWER_H_INCLUDED
#define WAVEFORMVIEWER_H_INCLUDED

#include "Playhead.h"
#include "Processors/TrackProcessor.h"
#include "Track.h"

class WaveformViewer : public Component,
                       private Track::Listener
{
public:
    WaveformViewer (OwnedArray<Track>& tracks);
    ~WaveformViewer();

    void paint (Graphics&) override;
    void resized() override;

    void setSelected (int selectedTrackIndex = -1) { selectedTrack = selectedTrackIndex; }

    void addTrack (Track* track);
    void deleteTrack (Track* track, int index);
    void trackColourChanged (Track* track, int index) override;

private:
    Array<TrackBase*> procs;
    OwnedArray<AudioThumbnailCache> caches;
    OwnedArray<AudioThumbnail> waveforms;
    Array<Colour> colours;

    std::unique_ptr<Playhead> playhead;

    int selectedTrack = -1;
    //MarkerView marker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformViewer)
};

#endif //WAVEFORMVIEWER_H_INCLUDED
