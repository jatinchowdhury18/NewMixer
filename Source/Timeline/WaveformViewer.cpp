#include "WaveformViewer.h"
#include "Track.h"

enum
{
    cacheSize = 5,
    thumbSamples = 1024,
    hash = 0x2345,
};

WaveformViewer::WaveformViewer (OwnedArray<Track>& tracks)
{
    for (auto track : tracks)
    {
        auto proc = dynamic_cast<TrackProcessor*> (track->getProcessor());
        if (proc == nullptr)
            continue; //@TODO: figure something out for input tracks

        procs.add (proc);
    
        caches.add (new AudioThumbnailCache (cacheSize));
        waveforms.add (new AudioThumbnail (thumbSamples, proc->getFormatManager(), *caches.getLast()));

        waveforms.getLast()->setReader (proc->getReader(), hash);

        colours.add (track->getColour());
    }

    playhead.reset (new Playhead (procs));

    addAndMakeVisible (playhead.get());

    //addAndMakeVisible (marker);
}

WaveformViewer::~WaveformViewer()
{
    for (int i = 0; i < waveforms.size(); i++)
    {
        waveforms[i]->clear();
        caches[i]->clear();
        waveforms[i]->setReader (nullptr, 0);
    }
}

void WaveformViewer::paint (Graphics& g)
{

    if (selectedTrack < 0)
    {
        g.setColour (Colours::indianred);

        for (int i = 0; i < waveforms.size(); i++)
        {
            const auto zoom = powf (procs[i]->gain(), 0.4f);
            waveforms[i]->drawChannels (g, getLocalBounds(), 0, waveforms[i]->getTotalLength(), zoom);
        }
    }
    else
    {
        g.setColour (colours[selectedTrack]);

        const auto zoom = powf (procs[selectedTrack]->gain(), 0.4f);
        waveforms[selectedTrack]->drawChannels (g, getLocalBounds(), 0, waveforms[selectedTrack]->getTotalLength(), zoom);
    }
}

void WaveformViewer::resized()
{
    const auto bounds = getLocalBounds();
    playhead->setBounds (bounds);
}

void WaveformViewer::addTrack (Track* track)
{
    auto proc = dynamic_cast<TrackProcessor*> (track->getProcessor());
    if (proc == nullptr)
        return; //@TODO: figure something out for input tracks

    procs.add (proc);

    caches.add (new AudioThumbnailCache (cacheSize));
    waveforms.add (new AudioThumbnail (thumbSamples, proc->getFormatManager(), *caches.getLast()));

    waveforms.getLast()->setReader (proc->getReader(), hash);

    playhead->addProc (proc);

    colours.add (track->getColour());
}

void WaveformViewer::deleteTrack (int index)
{
    if (selectedTrack < 0)
        return;

    if (index == selectedTrack)
        selectedTrack = -1;

    waveforms[index]->clear();
    caches[index]->clear();
    waveforms[index]->setReader (nullptr, 0);

    procs.remove (index);
    caches.remove (index);
    waveforms.remove (index);
    colours.remove (index);

    playhead->deleteProc (index);
}
