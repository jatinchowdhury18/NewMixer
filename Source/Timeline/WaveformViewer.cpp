#include "WaveformViewer.h"

WaveformViewer::WaveformViewer (TrackProcessor* trackProc) : 
    cache (5),
    waveform (512, trackProc->getFormatManager(), cache),
    playhead (trackProc)
    //marker (player)
{
    waveform.setReader (trackProc->getReader(), 0x2345);

    addAndMakeVisible (playhead);

    //addAndMakeVisible (marker);
}

WaveformViewer::~WaveformViewer()
{
    waveform.clear();
    cache.clear();
    waveform.setReader (nullptr, 0);
}

void WaveformViewer::paint (Graphics& g)
{
    g.setColour (Colours::indianred);

    waveform.drawChannels (g, getLocalBounds(), 0, waveform.getTotalLength(), 1.0f);
}

void WaveformViewer::resized()
{
    const auto bounds = getLocalBounds();
    playhead.setBounds (bounds);
}
